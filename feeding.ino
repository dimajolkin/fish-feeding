#include <Servo.h>

#define PORT_SERVO 7
#define PORT_LUMEN 6

#define CMD_EMPTY ""

#define CMD_START "start"
#define CMD_STOP "stop"
#define CMD_STATUS "status"

#define CMD_LUMEN_ON "lumen start"
#define CMD_LUMEN_OFF "lumen stop"
#define CMD_LUMEN_STATUS "lumen status"

#define CMD_RESTART "restart"

#define MAX_ROTATATIONS 500

String cmd = CMD_EMPTY;


void(* resetFunc) (void) = 0;

struct Loger {

  void init()
  {
    Serial.begin(9600);
  }

  void println(String log)
  {
     Serial.println(log);
  }

  void println(String log, int value)
  {
     Serial.print(log);
     Serial.println(value);
  }

  void print(String log)
  {
       Serial.print(log);
  }

  void print(String log, int value)
  {
       Serial.print(log);
        Serial.print(value);
  }
  
  void nl()
  {
       Serial.println("");
  }
};

//global loger
Loger loger;


struct Lumen {
   int port;
   int _status = 0;
   
   Lumen(int p):
   port(p)
   {
       pinMode(port, OUTPUT);     
       off();
   }

   void init()
   {
       loger.println("Init lumen v1.0");
       loger.println("cmd: lumen start|stop|status");
   }

   void on()
   {
      _status = 1;
      digitalWrite(port, LOW);
   }

   void off()
   {
      _status = 0;
      digitalWrite(port, HIGH);
   }

   int status()
   {
      return _status;
   }

   void printStatus()
   {
      if (_status == 1) {
         loger.println("lumen status: start");
      } else if (_status == 0) {
        loger.println("lumen status: stop");
      }
   }
};

struct Feeding {
  Servo s;
  int _port;
  int pos = 0;
  int breakPos = 360;
  int rotations = 0;

  Feeding(int p):
  _port(p)
  {
  }

  void start()
  {
     rotations = 0; 
     loger.println("prepare for started");
     delay(1000);
     loger.println("... feeding process started");
     s.attach(_port);
  }

  
  void stop() 
  {
     rotations = 0;
     if (!s.attached()) {
        loger.println("I not work");
     } else {
        s.detach();
        loger.println("stop feeding process");  
     } 
  }

  void init()
  {
      loger.println("Init fooder v1.0");
      loger.println("cmd: start|stop|status|restart");
  }

  void update()
  {
     if (!s.attached()) {
       return;
     }
  
     if (pos > 70) {
       pos = 0;
       rotations++;
     }
     
     s.write(pos++);  
  }

  bool status()
  {
    return s.attached();
  }

  bool check()
  {
    if (!s.attached()) {
      return true;
    }

    if (rotations > MAX_ROTATATIONS) {
       return false;
    }
     
    return true;
  }

  void printlnStatus()
  {
    loger.print("feeding status:");
    if (s.attached()) {
        loger.print("running");
    } else {
        loger.print("stop");  
    }
    loger.print(", rotations :", rotations);  
    loger.nl();
  }
};


Lumen lumen(PORT_LUMEN);
Feeding feeding(PORT_SERVO);

void setup() {
  loger.init();

  feeding.init();
  lumen.init();
 
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

SIGNAL(TIMER0_COMPA_vect) 
{
    feeding.update();
}

void loop() {

  if (Serial.available()) {
      cmd = Serial.readString();
      loger.println("cmd: " + cmd);
      if (cmd == CMD_START) {
        feeding.start();
      } 
      
      if (cmd == CMD_STOP) {
        feeding.stop();
      }

      if (cmd == CMD_LUMEN_ON) {
        
        lumen.on();
        loger.println(cmd + " status: ", lumen.status());
      }

      if (cmd == CMD_LUMEN_OFF) {
        lumen.off();
        loger.println(cmd + " status: ", lumen.status());
      }
         
      delay(100);
}

  if (cmd == CMD_LUMEN_STATUS) {
     lumen.printStatus();
     cmd = CMD_EMPTY;
  }

  if (cmd == CMD_STATUS) {
       feeding.printlnStatus();    
       cmd = CMD_EMPTY;
  }
  
  if (cmd == CMD_RESTART) {
     loger.println("feeding restart");
      resetFunc();
    cmd = CMD_EMPTY;
  }

  if (!feeding.check()) {
      feeding.stop();
      cmd = CMD_EMPTY;
  }
}
