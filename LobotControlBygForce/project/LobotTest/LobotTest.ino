/*
   Copyright 2017, OYMotion Inc.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
   OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
   AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
   THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
   DAMAGE.

*/

#include "LobotServoController.h"
#include "gForceAdapter.h"

#define thumb         0
#define indexfinger   1
#define middlefinger  2
#define ringfinger    3
#define littlefinger  4
#define wrist         5
#define armtwist      6
#define moveTime    150

#define ServoSerial Serial3
#define gforceSerial Serial2


int gesture[7][6] = {{2500,2500,2300,800,1000,1500},   // fist
                  {1200,1200,1100,1900,1900,1500},    // spread
                  {1200,1200,1100,1900,1900,1900},    // wave in
                  {1200,1200,1100,1900,1900,1100},    // wave out
                  {2000,1900,1800,1300,1500,1500},    // pinch
                  {1200,1200,2200,800,1100,1500},      // shoot
                  {1400,1400,1400,1500,1650,1500}};    // release

enum GestureType
{
  FistIdx = 0,
  ReleaseIdx,
  WaveInIdx,
  WaveOutIdx,
  TapIdx,
  ShootIdx,
  RelaxIdx
};      
            
LobotServoController myse(ServoSerial);
GForceAdapter gforce(&gforceSerial);


unsigned long timeStamp;
GestureType last_gesIdx;
void perform_gesture(GestureType gestureIdx);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  ServoSerial.begin(9600);
  gforceSerial.begin(115200);
  delay(500);
  timeStamp = millis();
}

void loop() {
   //put your main code here, to run repeatedly:
  if((millis() - timeStamp)>5000)
  {
    timeStamp = millis();
  }
  myse.receiveHandle();
  GF_Data gForceData;
  if(OK == gforce.GetGForceData(&gForceData)) 
  {
                GF_Gesture gesture;
                switch (gForceData.type){
                      case GF_Data::QUATERNION:
                          break;
                      case GF_Data::GESTURE:
                          gesture = gForceData.value.gesture;
                          if(gesture == GF_FIST) {
                                perform_gesture(FistIdx);
                          } else if(gesture == GF_SPREAD) {
                                perform_gesture(ReleaseIdx);
                          } else if(gesture == GF_WAVEIN) {
                                perform_gesture(WaveInIdx);
                          } else if(gesture == GF_WAVEOUT) {
                                perform_gesture(WaveOutIdx);
                          } else if(gesture == GF_PINCH){
                                perform_gesture(TapIdx);
                          } else if(gesture == GF_SHOOT) {
                                perform_gesture(ShootIdx);
                          } else if(gesture == GF_RELEASE) {
                                perform_gesture(RelaxIdx);
                          } else if(gesture == GF_UNKNOWN) {
                                perform_gesture(RelaxIdx);
                          }
                          break;
                      default:
                           break;
                }
      }
}

void perform_gesture(GestureType gestureIdx)
{
  int idx = gestureIdx;
  LobotServo servos[6];
  if(gestureIdx != last_gesIdx)
  {
    for(int i=0;i<6;i++)
    {
      servos[i].ID = i;
      servos[i].Position = gesture[idx][i];
      Serial.print(gesture[idx][i]);Serial.print(",");
    }
    myse.moveServos(servos,6,moveTime);
    last_gesIdx = idx;
  }

  Serial.print("Gesture ");Serial.println(gestureIdx);
}

