// AquesTalkTTS サンプルプログラム
// あらかじめ、AquesTalk-ESP32ライブラリをインストール（ビルド準備）し、
// 辞書データ(aqdic_m.bin)をSDメモリカードに保存しておくこと
#include <M5Stack.h>
#include "AquesTalkTTS.h"

const char* licencekey = "XXX-XXX-XXX";	// AquesTalk-ESP32 licencekey

void setup()
{
  int iret;
  Serial.begin(115200);
  M5.begin();
  
  iret = TTS.createK(licencekey);
  if(iret){
       Serial.print("ERR: TTS_createK():");
       Serial.println(iret);
  }
}

void loop()
{
	static bool bCountup=false;
	static int  mday=1;

  if (Serial.available()) {
    char kstr[256];
    size_t len = Serial.readBytesUntil('\r', kstr, 256);
    kstr[len]=0;
		int iret = TTS.playK(kstr, 100);
    if(iret){
      Serial.print("ERR:TTS.playK()=");
      Serial.println(iret);
    }
	}
	
	if(M5.BtnA.wasPressed()){
    TTS.playK("新しいライブラリは、漢字テキストから音声合成ができるようになりました。", 100);
	}
	else if(M5.BtnB.wasPressed()){
    bCountup = true;
	}
	else if(M5.BtnC.wasPressed()){
		TTS.stop();
    bCountup = false;
	}

	if(bCountup){
		if(TTS.isPlay()==false){
			char koe[64];
			sprintf(koe, "<NUMK VAL=%d COUNTER=nichi>.",mday);
			TTS.play(koe, 100);
			mday++;
			if(mday>31) mday=1;
		}
	}
	
  M5.update();
}

