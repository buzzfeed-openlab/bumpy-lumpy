#define MIC_PIN   A0  // Microphone is attached to this analog pin
#define DC_OFFSET  0  // DC offset in mic signal - if unusure, leave 0
#define NOISE     10  // Noise/hum/interference in mic signal
#define SAMPLES   60  // Length of buffer for dynamic level adjustment

#define VIB_PIN   A7

#define SIGNAL_PIN D7

// time values
int
    vib_off_time = 1000, // period after it initially comes on that it stops taking vibration readings, regardless of if car passes or not
    car_pass_time = 5000, // the period of time after it counts a car that it can't take more vibration or volume readings
    sleep_time = 1000*60*15, // amount of time that is allowed to pass before it goes to sleep automatically.
    state_switch=0, // the time that the state last switched
    state_time; // amount of time that has passed in the current state so far, calculate with millis()-state_switch

int vibInput,
    micInput;

// other values
int mic_threshold = 800; // below which the publish is triggered, aka volume created by a car running over it
int state;
char szInfo[64];


int
  vol[SAMPLES],       // Collection of prior volume samples
  lvl       = 10,      // Current "dampened" audio level
  minLvlAvg = 0,      // For dynamic adjustment of graph low & high
  maxLvlAvg = 1024;

void setup() {
    Serial.begin(9600);

    state=1;

    state_time=millis()-state_switch;

    Particle.function("read",readMic);
    Particle.function("off",mySleep);

    pinMode(VIB_PIN,INPUT_PULLDOWN);
    pinMode(SIGNAL_PIN,OUTPUT);

    Particle.publish("car","reset",PRIVATE);
}

void loop() {
    state_time=millis()-state_switch;

    vibInput=digitalRead(VIB_PIN);
    micInput=readMic("");

    state=getState();

    if (state==3) {
        sprintf(szInfo, "%2.2f", micInput);
        Particle.publish("car",szInfo,PRIVATE);
        digitalWrite(SIGNAL_PIN,HIGH);
    }
    else if (state==4) {
        digitalWrite(SIGNAL_PIN,LOW);
    }
    else if (state==6) {
        mySleep("");
    }

//    Serial.println(readMic(""));
    Serial.println(state);

    delay(50);
}

int getState() {
    // 0 is everything off
    // 1 is vibration triggered (first turned on) and not taking vib readings (refractory period)
    // 2 is taking vibration readings again
    // 3 is car threshold triggered
    // 4 is time after car threshold triggered that new cars can't be recorded (car_pass_time)
    // 5 is waiting for the next thing to happen, like a vibration to trigger.
    // 6 is when nothing has happened in a while and it is time to go to sleep in the loop.

    if (state==0) {
        state=1;
        state_switch=millis();
    }

    // if in state 1 and vib_off_time passes, then switch to state 2
    // if in state 1 and car threshold triggered, then output 3 (and in the loop, sense 3 and publish stuff)

    if (state==1) {
        if (state_time>vib_off_time) {
            state=2;
            state_switch=millis();
        }
        else if (micInput<mic_threshold) {
            state=3;
            state_switch=millis();
        }
    }

    // if in state 2 and vibration, reset state timer and move to 1

    else if (state==2) {
        if (vibInput==1) {
            state=1;
            state_switch=millis();
        }
    }

    // if in state 3, move to state 4

    else if (state==3) {
        state=4;
        state_switch=millis();
    }

    // if in state 4 and car_pass_time passes, then switch to state 5

    else if (state==4) {
        if (state_time>car_pass_time) {
            state=5;
            state_switch=millis();
        }
    }

    // if in state 5 and vibration occurs, then go back to state 1
    // if in state 5 and sleep_time passes, then output state 6 (and in the loop, go to sleep!)

    else if (state==5) {
        if (vibInput==1) {
            state=1;
            state_switch=millis();
        }
        if (state_time>sleep_time) {
            state=6;
            state_switch=millis();
        }
    }

    // state 6 has no output because as soon as loop recognizes state 6, it goes to sleep.

    return state;

}

int mySleep(String command) {
  System.sleep(SLEEP_MODE_DEEP, 999999999);
  state=0;
  return 0;
}

int readMic(String command) {
    int n;
    n   = analogRead(MIC_PIN);                        // Raw reading from mic
    n   = abs(n - 512 - DC_OFFSET); // Center on zero
    n   = (n <= NOISE) ? 0 : (n - NOISE);             // Remove noise/hum
    lvl = ((lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)
//    Serial.println(lvl);
    return n;
}
