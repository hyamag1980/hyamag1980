#define TCSWB 15
#define TCSWW 16
#define VR 0
#define LED 5
#define CLEDR 6
#define CLEDG 9
#define CLEDB 10
#define SPK 11

void setup()
{
    pinMode(TCSWB, INPUT);
    pinMode(TCSWW, INPUT);
    pinMode(SPK, OUTPUT);
    pinMode(LED, OUTPUT);
    pinMode(CLEDR, OUTPUT);
    pinMode(CLEDG, OUTPUT);
    pinMode(CLEDB, OUTPUT);

    Serial.begin(115200);
}

int tcsww_sw = 0;        //押している間は1、離している間は0
int tcsww_on_edge = 0;   //押した瞬間に1、その他は0
int tcsww_off_edge = 0;  //離した瞬間に1、その他は0
void tcsww_sense()
{
    static unsigned long time;

    tcsww_on_edge = 0;
    tcsww_off_edge = 0;
    if(millis() - time > 20) {
        time = millis();

        if(digitalRead(TCSWW) == LOW) {
            if(tcsww_sw == 0) {
                tcsww_on_edge = 1;
            }
            tcsww_sw = 1;
        } else {
            if(tcsww_sw == 1) {
                tcsww_off_edge = 1;
            }
            tcsww_sw = 0;
        }
    }
}

int tcswb_sw = 0;        //押している間は1、離している間は0
int tcswb_on_edge = 0;   //押した瞬間に1、その他は0
int tcswb_off_edge = 0;  //離した瞬間に1、その他は0
void tcswb_sense()
{
    static unsigned long time;

    tcswb_on_edge = 0;
    tcswb_off_edge = 0;
    if(millis() - time > 20) {
        time = millis();

        if(digitalRead(TCSWB) == HIGH) {
            if(tcswb_sw == 0) {
                tcswb_on_edge = 1;
            }
            tcswb_sw = 1;
        } else {
            if(tcswb_sw == 1) {
                tcswb_off_edge = 1;
            }
            tcswb_sw = 0;
        }
    }
}

void led_light(int r)
{
    analogWrite(LED, r);
}

void cled_light(int r, int g, int b)
{
    analogWrite(CLEDR, r);
    analogWrite(CLEDG, g);
    analogWrite(CLEDB, b);
}

void spk_ring(int hz)
{
    tone(SPK, hz);
}

void spk_ring(int hz, int ms)
{
    tone(SPK, hz, ms);
}

void spk_stop()
{
    noTone(SPK);
}

int vr_read()
{
    return analogRead(VR);
}
/********************************
 応用問題1　クリック，ダブルクリック
 タクトスイッチ白をクリックするとカラーLEDが赤で０．５秒間隔で点滅する。
 タクトスイッチ白をダブルクリックするとカラーLEDが緑で０．１秒間隔で点滅する。
 タクトスイッチ黒を押して離すと（押した瞬間），カラーLEDは消灯する。
 以上の動作は繰り返し実行できること。
 ********************************/
void loop()
{
    static int click_state = 0;
    static int state = 0;
    static unsigned long time = 0;
    static unsigned long click_time = 0;
    static int click_count = 0;
    static int vr;

    tcsww_sense();
    tcswb_sense();
    vr = vr_read();

    if(click_state == 0){
        if(tcsww_on_edge){
            click_state = 1;
            click_time = millis();
            click_count = 1;
        }
    }else if(click_state == 1){
        if(tcsww_on_edge){
            click_count++;
        }
        if(millis() - click_time > 500){
            if(click_count == 1){
                state = 1;
                click_time = millis();
            }else if(click_count == 2){
                state = 2;
                click_time = millis();
            }
            click_state = 0;
        }
    }

    if(state == 0){
        cled_light(0,0,0);
    }else if(state == 1){
        if(millis() - time < 500){
            cled_light(255,0,0);
        }else if(millis() - time < 1000){
            cled_light(0,0,0);
        }else{
            time = millis();
        }
        if(tcswb_on_edge){
            cled_light(0,0,0);
            state = 0;
        }
    }else if(state == 2){
        if(millis() - time < 100){
            cled_light(0,255,0);
        }else if(millis() - time < 200){
            cled_light(0,0,0);
        }else{
            time = millis();
        }
        if(tcswb_on_edge){
            cled_light(0,0,0);
            state = 0;
        }
    }

    led_light(vr / 4);
}
