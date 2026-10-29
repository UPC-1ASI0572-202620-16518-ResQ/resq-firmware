/*
 * Frutos del Chira / G1 / D5: demostracion Wokwi
 * Responsable: Fabricio Fabian Quispe Barzola
 * PRUEBA DIDACTICA: WAN=0, WAN=1, NODE=D1, EVENT=CRIT, STATUS.
 * La WAN y los ACK remotos se EMULAN: no hay servidor ni modem real.
 * Los HC-SR04 de Wokwi representan funcionalmente los JSN-SR04T.
 * Los interruptores de flotador, lluvia y la entrada D1-D4 se simulan.
 * microSD: almacenamiento real dentro de la sesion de Wokwi;
 * no se afirma persistencia entre sesiones web diferentes.
 */
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>

namespace Pins {
  constexpr uint8_t TRIG[4] = {13, 14, 25, 26};
  constexpr uint8_t ECHO[4] = {32, 33, 34, 35};
  constexpr uint8_t RAIN[2] = {36, 39};  // Potenciometros ADC1 (en vez de FC-37)
  constexpr uint8_t FLOATS[2] = {16, 17}; // Interruptores hacia GND
  constexpr uint8_t SD_CS = 5;
  constexpr uint8_t SDA = 21;
  constexpr uint8_t SCL = 22;
}

RTC_DS1307 rtc;  // Solo Wokwi: la version fisica propuesta es DS3231
bool sdOk = false;
bool rtcOk = false;
bool wanUp = true;            // enlace EMULADO por monitor serie
uint32_t nextSeq = 1;
uint32_t invalidRows = 0;
uint32_t lastSonic = 0, lastSnapshot = 0, lastHeartbeat = 0, lastSend = 0, lastPrint = 0;
uint8_t sonicIndex = 0;
float distanceCm[4] = {NAN,NAN,NAN,NAN};
uint8_t faultStreak[4] = {0,0,0,0};
const uint16_t RAIN_THRESHOLD = 1700; // demostracion; calibrar sensor fisico
const float WATER_CRITICAL_CM = 40.0f; // distancia al agua segun altura de montaje
const uint32_t SONIC_INTERVAL_MS = 400; // mide una sonda a la vez, limita interferencia
const uint32_t SNAPSHOT_INTERVAL_MS = 15000;
const uint32_t HEARTBEAT_MS = 10000;
const uint32_t SEND_INTERVAL_MS = 2500;
const size_t SD_WARNING_BYTES = 5UL * 1024 * 1024;
const char* EVENTS = "/events.log";
const char* ACKS = "/acks.log";

enum class RiskState { NORMAL, ALERTA, CRITICO, FALLA_SENSOR };
RiskState current = RiskState::NORMAL;

uint32_t fnv1a(const String& s) {
  uint32_t h = 2166136261UL;
  for (size_t i=0; i<s.length(); ++i) { h ^= static_cast<uint8_t>(s[i]); h *= 16777619UL; }
  return h; // integridad accidental; NO firma criptografica
}

String utcStamp() {
  if (!rtcOk) return "RTC_NO_DISPONIBLE";
  DateTime t = rtc.now();
  if (t.year() < 2024) return "RTC_SIN_AJUSTAR";
  char b[26];
  snprintf(b, sizeof(b), "%04d-%02d-%02dT%02d:%02d:%02dZ",
           t.year(),t.month(),t.day(),t.hour(),t.minute(),t.second());
  return String(b);
}

bool appendLine(const char* path, const String& line) {
  if (!sdOk) return false;
  File f = SD.open(path, FILE_APPEND);
  if (!f) return false;
  bool ok = f.println(line) == line.length()+2; // CRLF de Arduino Print::println
  f.flush();
  f.close();
  // No asumimos atomicidad de escritura ante perdida abrupta de energia.
  return ok;
}

bool validEvent(const String& line, uint32_t* idOut=nullptr, uint8_t* priorityOut=nullptr) {
  int last=line.lastIndexOf('|');
  if (last<0) return false;
  String raw=line.substring(0,last), crc=line.substring(last+1);
  if (crc.length()==0 || strtoul(crc.c_str(),nullptr,16)!=fnv1a(raw)) return false;
  int first=raw.indexOf('|');
  if (first<1) return false;
  uint32_t id=strtoul(raw.substring(0,first).c_str(),nullptr,10);
  int second=raw.indexOf('|',first+1), third=(second>=0?raw.indexOf('|',second+1):-1);
  if (third<0) return false;
  uint8_t priority=static_cast<uint8_t>(raw.substring(second+1,third).toInt());
  if (priority>3 || id==0) return false;
  if (idOut) *idOut=id;
  if (priorityOut) *priorityOut=priority;
  return true;
}

void restoreSequence() {
  if (!sdOk || !SD.exists(EVENTS)) return;
  File f=SD.open(EVENTS,FILE_READ);
  while (f && f.available()) {
    String line=f.readStringUntil('\n'); line.trim();
    uint32_t id=0;
    if (validEvent(line,&id,nullptr) && id>=nextSeq) nextSeq=id+1;
    else if (line.length()) invalidRows++;
  }
  if (f) f.close();
  Serial.printf("[RECOVERY] secuencia siguiente=%lu | filas invalidas=%lu\n",
    (unsigned long)nextSeq,(unsigned long)invalidRows);
}

bool isAcked(uint32_t id) {
  if (!sdOk || !SD.exists(ACKS)) return false;
  File f=SD.open(ACKS,FILE_READ);
  while(f && f.available()) {
    String line=f.readStringUntil('\n'); line.trim();
    int sep=line.indexOf('|');
    if(sep>0) {
      String left=line.substring(0,sep),crc=line.substring(sep+1);
      if(static_cast<uint32_t>(left.toInt())==id && strtoul(crc.c_str(),nullptr,16)==fnv1a(left)) {
        f.close(); return true;
      }
    }
  }
  if(f) f.close();
  return false;
}

bool persistAck(uint32_t id) {
  String payload=String(id);
  String line=payload+"|"+String(fnv1a(payload),HEX);
  return appendLine(ACKS,line);
}

bool event(const String& node, const String& type, uint8_t priority, const String& value) {
  if (!sdOk) {
    Serial.println("[FALLO] No hay SD; ALARMA LOCAL SI, persistencia NO");
    return false;
  }
  File usage=SD.open(EVENTS,FILE_READ);
  size_t used=usage?usage.size():0;
  if(usage) usage.close();
  if(used>SD_WARNING_BYTES && priority>=2) {
    Serial.println("[SD] UMBRAL: evento no critico rechazado; intervencion requerida");
    return false;
  }
  uint32_t id=nextSeq;
  String safeValue=value; safeValue.replace('|','/'); safeValue.replace('\n',' ');
  String payload=String(id)+"|"+node+"|"+String(priority)+"|"+utcStamp()+"|"+type+"|"+safeValue;
  String line=payload+"|"+String(fnv1a(payload),HEX);
  if(!appendLine(EVENTS,line)) { Serial.println("[SD] Error al guardar; NO enviar sin registro"); return false; }
  nextSeq++;
  Serial.printf("[STORE] id=%lu P%u nodo=%s tipo=%s WAN=%s\n",(unsigned long)id,priority,node.c_str(),type.c_str(),wanUp?"UP":"DOWN");
  return true;
}

bool sendOne() {
  if(!wanUp || !sdOk || !SD.exists(EVENTS)) return false;
  File f=SD.open(EVENTS,FILE_READ);
  String best="";
  uint8_t top=255;
  uint32_t bestId=0;
  while (f && f.available()) {
    String line=f.readStringUntil('\n'); line.trim();
    uint32_t id=0; uint8_t priority=0;
    if(!validEvent(line,&id,&priority) || isAcked(id)) continue;
    if(priority<top) { top=priority; best=line; bestId=id; if(top==0) break; }
  }
  if(f) f.close();
  if(best.length()==0) return false;
  // NO HAY conexion real: ack remoto y envio se emulan deliberadamente.
  Serial.printf("[TX-SIM] id=%lu P%u -> receptor remoto de prueba\n",(unsigned long)bestId,top);
  bool ackRemoteSimulated=true;
  if(ackRemoteSimulated && persistAck(bestId)) {
    Serial.printf("[ACK-SIM] id=%lu confirmado en log SD\n",(unsigned long)bestId);
    return true;
  }
  Serial.printf("[PENDIENTE] id=%lu no confirmado, reintento posterior\n",(unsigned long)bestId);
  return false;
}

uint32_t pendingCount() {
  if(!sdOk || !SD.exists(EVENTS)) return 0;
  File f=SD.open(EVENTS,FILE_READ);
  uint32_t n=0;
  while(f && f.available()) {
    String line=f.readStringUntil('\n'); line.trim(); uint32_t id=0;
    if(validEvent(line,&id,nullptr) && !isAcked(id)) n++;
  }
  if(f) f.close();
  return n;
}

void sampleUltrasonic() {
  uint8_t i=sonicIndex;
  digitalWrite(Pins::TRIG[i],LOW); delayMicroseconds(3);
  digitalWrite(Pins::TRIG[i],HIGH); delayMicroseconds(12);
  digitalWrite(Pins::TRIG[i],LOW);
  unsigned long microseconds=pulseIn(Pins::ECHO[i],HIGH,26000UL); // timeout acotado
  float cm=microseconds?static_cast<float>(microseconds)/58.0f:NAN;
  if(!isfinite(cm) || cm<25 || cm>450) {
    if(faultStreak[i]<255) faultStreak[i]++;
    distanceCm[i]=NAN;
  } else {distanceCm[i]=cm; faultStreak[i]=0;}
  sonicIndex=(sonicIndex+1)%4;
}

const char* stateName(RiskState s) {
  switch(s) {
    case RiskState::NORMAL:return "NORMAL";
    case RiskState::ALERTA:return "ALERTA";
    case RiskState::CRITICO:return "CRITICO";
    default:return "FALLA_SENSOR";
  }
}

void evaluate() {
  bool critical = digitalRead(Pins::FLOATS[0])==LOW || digitalRead(Pins::FLOATS[1])==LOW;
  bool raining = analogRead(Pins::RAIN[0])<RAIN_THRESHOLD || analogRead(Pins::RAIN[1])<RAIN_THRESHOLD;
  bool failure=false;
  for(int i=0;i<4;i++) {
    if(isfinite(distanceCm[i]) && distanceCm[i] < WATER_CRITICAL_CM) critical=true;
    if(faultStreak[i]>=3) failure=true;
  }
  // Riesgo fisico primero; falla de sensor se informa tambien como evento independiente.
  RiskState next=critical?RiskState::CRITICO:(failure?RiskState::FALLA_SENSOR:(raining?RiskState::ALERTA:RiskState::NORMAL));
  if(next!=current) {
    RiskState old=current; current=next;
    Serial.printf("[ESTADO] %s -> %s\n",stateName(old),stateName(current));
    event("D5","CAMBIO_ESTADO",current==RiskState::CRITICO?0:1,stateName(current));
  }
  if(critical) Serial.println("[ALARMA LOCAL] ALERTA CRITICA: VERIFICAR NIVEL DE AGUA");
}

void snapshot() {
  char payload[150];
  snprintf(payload,sizeof(payload),"U1=%.0f;U2=%.0f;U3=%.0f;U4=%.0f;R1=%d;R2=%d;F1=%d;F2=%d;S=%s",
    distanceCm[0],distanceCm[1],distanceCm[2],distanceCm[3],
    analogRead(Pins::RAIN[0]),analogRead(Pins::RAIN[1]),
    digitalRead(Pins::FLOATS[0])==LOW,digitalRead(Pins::FLOATS[1])==LOW,stateName(current));
  event("D5","TELEMETRIA",2,String(payload));
}

void handleCommand(String s) {
  s.trim(); s.toUpperCase();
  if(s=="WAN=0") {wanUp=false; Serial.println("[WAN] Principal y respaldo emulados como CAIDOS");}
  else if(s=="WAN=1") {wanUp=true; Serial.println("[WAN] Enlace emulado RESTAURADO");}
  else if(s=="EVENT=CRIT") {event("D5","INUNDACION_PRUEBA",0,"FLOTADOR_ACTIVO");}
  else if(s=="NODE=D1" || s=="NODE=D2" || s=="NODE=D3" || s=="NODE=D4") {
    event(s.substring(5),"ALERTA_NODO",1,"MENSAJE_RS485_EMULADO");
  }
  else if(s=="STATUS") {
    Serial.printf("[STATUS] riesgo=%s WAN=%s SD=%s RTC=%s pendientes=%lu\n",
      stateName(current),wanUp?"UP":"DOWN",sdOk?"OK":"FAIL",rtcOk?"OK":"FAIL",(unsigned long)pendingCount());
  }
  else if(s=="HELP") Serial.println("Comandos: WAN=0 | WAN=1 | EVENT=CRIT | NODE=D1..D4 | STATUS | HELP");
  else if(s.length()) Serial.println("[CMD] Desconocido; enviar HELP");
}

void setup() {
  Serial.begin(115200); Serial.setTimeout(70);
  for(uint8_t i=0;i<4;i++){pinMode(Pins::TRIG[i],OUTPUT);pinMode(Pins::ECHO[i],INPUT); digitalWrite(Pins::TRIG[i],LOW);}
  for(uint8_t i=0;i<2;i++) pinMode(Pins::FLOATS[i],INPUT_PULLUP);
  pinMode(Pins::RAIN[0],INPUT);pinMode(Pins::RAIN[1],INPUT);
  Wire.begin(Pins::SDA,Pins::SCL);
  rtcOk=rtc.begin();
  if(rtcOk && !rtc.isrunning()) {Serial.println("[RTC] DS1307 detenido; hora no confiable"); rtcOk=false;}
  SPI.begin(18,19,23,Pins::SD_CS);
  sdOk=SD.begin(Pins::SD_CS,SPI,8000000);
  Serial.println("==========================================");
  Serial.println("D5 Frutos del Chira - Fabricio Fabian Quispe Barzola");
  Serial.printf("microSD=%s RTC=%s (DEMO, WAN/ACK simulados)\n",sdOk?"OK":"ERROR",rtcOk?"OK":"ERROR");
  restoreSequence();
  Serial.println("Enviar HELP en el monitor serie; Newline, 115200 baudios");
  event("D5","ARRANQUE",1,"GATEWAY_BOOT");
}

void loop() {
  if(Serial.available()) handleCommand(Serial.readStringUntil('\n'));
  uint32_t now=millis();
  if(now-lastSonic>=SONIC_INTERVAL_MS){lastSonic=now; sampleUltrasonic();evaluate();}
  if(now-lastSnapshot>=SNAPSHOT_INTERVAL_MS){lastSnapshot=now; snapshot();}
  if(now-lastHeartbeat>=HEARTBEAT_MS){lastHeartbeat=now;event("D5","HEARTBEAT",2,stateName(current));}
  if(now-lastSend>=SEND_INTERVAL_MS){lastSend=now;sendOne();}
  if(now-lastPrint>=12000){lastPrint=now;Serial.printf("[RESUMEN] WAN=%d enCola=%lu estado=%s\n",wanUp,(unsigned long)pendingCount(),stateName(current));}
  delay(5);
}
