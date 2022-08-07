// data for my own focuser
// minPosition = 0;
// maxPosition = 57600;
// nStepsPerMM = 590;

// Motor pins
// focuser 1
int STEPPER1_PIN_1= 2;
int STEPPER1_PIN_2= 3;
int STEPPER1_PIN_3= 4;
int STEPPER1_PIN_4= 5;

// focuser speeds in ms
int delay1 = 2;

// focuser number increment steps
int nbTurns1 = 1;

// new value
int newValue = 0;
unsigned long int currentPosition = 0;
unsigned long int targetPosition = 0;
int speed = 2; // not used
int isRunning = 0; // not useful because no multithreading

// convert hex string to integer base 10
unsigned long int x2i(String number) 
{
  unsigned long int x = 0;
  int len = number.length();
  
  for(int i=0;i<len;i++) 
  {
    char c = number[i];
    
    if (c >= '0' && c <= '9') 
    {
      x *= 16;
      x += c - '0'; 
    }
    else if (c >= 'A' && c <= 'F') 
    {
      x *= 16;
      x += (c - 'A') + 10; 
    }
    else break;
  }
  
  return x;
}

void setup()
{  
    // pin init
    pinMode(STEPPER1_PIN_1, OUTPUT);
    pinMode(STEPPER1_PIN_2, OUTPUT);
    pinMode(STEPPER1_PIN_3, OUTPUT);
    pinMode(STEPPER1_PIN_4, OUTPUT);
    Serial.begin(9600);
    Serial.flush();
}

void loop()
{
    String rawcmd = "";  // received full command (command + parameter + #)
    String cmd = "";     // command
    String param = "";   // param
    int len = 0;        

    if(Serial.available()>0)
    {
        rawcmd = Serial.readStringUntil('#');
        len = rawcmd.length();

        // recupération commande
        cmd = rawcmd.substring(1,3);

        // récupération paramètre
        if (len > 2)
        {
            param = rawcmd.substring(3,len);
        }
  
        // command processing below
        
        // firmware value, always return "10"
        if (cmd == "GV") 
        {
          Serial.print("25#");
        }

        // LED backlight (not used)
        if (cmd == "GB")
        {
          Serial.print("00#");
        }
        
        // get temperature (not used)
        if (cmd == "GT")
        {
          Serial.print("20#");
        }

        // get temperature coefficient (not used)
        if (cmd == "GC")
        {
          Serial.print("02#");
        }
        
        // get current motor position 
        if (cmd == "GP")
        {
            char tempString[6];
            sprintf(tempString, "%04X", currentPosition);
            Serial.print(tempString);
            Serial.print("#");
        }

        // get target position 
        if (cmd == "GN")
        {
            char tempString[6];
            sprintf(tempString, "%04X", targetPosition);
            Serial.print(tempString);
            Serial.print("#");
        }

        // get the current motor speed, only hex values of 02, 04, 08, 10, 20
        if (cmd == "GD") 
        {
            char tempString[6];
            sprintf(tempString, "%02X", speed);
            Serial.print(tempString);
            Serial.print("#");
        }

        // whether half-step is enabled or not (not used)
        if (cmd == "GH") 
        {
            Serial.print("00#");
        }

        // set speed, only acceptable hex values are 02, 04, 08, 10, 20
        if (cmd == "SD") 
        {
            if (param == "02")
              speed = 2;
            else if (param == "04")
              speed = 4;
            else if (param == "08")
              speed = 8;
            else if (param == "10")
              speed = 16;
            else if (param == "20")
              speed = 32;
        }

        // get if motor is running
        if (cmd == "GI") 
        { 
          if (isRunning)
              Serial.print("01#");
          else
              Serial.print("00#");
        }

        // set the current position (not used)
        if (cmd == "SP") 
        {
            // empty
        }

        // park the focuser, params are ignored
        if (cmd == "PH") 
        { 
            targetPosition = 0;
            if (currentPosition > targetPosition)
            {
                isRunning = 1;
                while (targetPosition < currentPosition)
                {
                  moveForward1();
                }
                isRunning = 0;
            }
        }



        // Set the target position
        if (cmd == "SN") 
        { 
            targetPosition = x2i(param);
        }
        
        // move focuser to the target position
        if (cmd == "FG") 
        { 
          if (targetPosition > currentPosition)
          {
              isRunning = 1;
              while (currentPosition < targetPosition)
              {
                moveBackward1();
              }
              isRunning = 0;
          }
          else if (currentPosition > targetPosition)
          {
              isRunning = 1;
              while (targetPosition < currentPosition)
              {
                moveForward1();
              }
              isRunning = 0;
          }
        }

        // stop moving (not used)
        if (cmd == "FQ") 
        { 
          // empty
        }
 
    }
}


void moveForward1()
{
    int nbTurns = 1;
    currentPosition--;
    
    // number of steps
    int step_number = 0;

    while(step_number < (nbTurns * 4))
    {
      moveCoilForward1(step_number%4);
      step_number++;
      delay(delay1);
    }

    digitalWrite(STEPPER1_PIN_1, LOW);
    digitalWrite(STEPPER1_PIN_2, LOW);
    digitalWrite(STEPPER1_PIN_3, LOW);
    digitalWrite(STEPPER1_PIN_4, LOW);
}

void moveBackward1()
{
    int nbTurns = 1;
    currentPosition++;
      
    // number of steps
    int step_number = 0;

    while(step_number < (nbTurns * 4))
    {
      moveCoilBackward1(step_number%4);
      step_number++;
      delay(delay1);
    }

    digitalWrite(STEPPER1_PIN_1, LOW);
    digitalWrite(STEPPER1_PIN_2, LOW);
    digitalWrite(STEPPER1_PIN_3, LOW);
    digitalWrite(STEPPER1_PIN_4, LOW);
}



void moveCoilForward1(int coil_number)
{
  switch(coil_number)
  {
      case 0:
        digitalWrite(STEPPER1_PIN_1, HIGH);
        digitalWrite(STEPPER1_PIN_2, LOW);
        digitalWrite(STEPPER1_PIN_3, LOW);
        digitalWrite(STEPPER1_PIN_4, LOW);
        break;
      case 1:
        digitalWrite(STEPPER1_PIN_1, LOW);
        digitalWrite(STEPPER1_PIN_2, HIGH);
        digitalWrite(STEPPER1_PIN_3, LOW);
        digitalWrite(STEPPER1_PIN_4, LOW);
        break;
      case 2:
        digitalWrite(STEPPER1_PIN_1, LOW);
        digitalWrite(STEPPER1_PIN_2, LOW);
        digitalWrite(STEPPER1_PIN_3, HIGH);
        digitalWrite(STEPPER1_PIN_4, LOW);
        break;
      case 3:
        digitalWrite(STEPPER1_PIN_1, LOW);
        digitalWrite(STEPPER1_PIN_2, LOW);
        digitalWrite(STEPPER1_PIN_3, LOW);
        digitalWrite(STEPPER1_PIN_4, HIGH);
        break;
  } 
}

void moveCoilBackward1(int coil_number)
{
  switch(coil_number)
  {
    case 0:
      digitalWrite(STEPPER1_PIN_1, LOW);
      digitalWrite(STEPPER1_PIN_2, LOW);
      digitalWrite(STEPPER1_PIN_3, LOW);
      digitalWrite(STEPPER1_PIN_4, HIGH);
      break;
    case 1:
      digitalWrite(STEPPER1_PIN_1, LOW);
      digitalWrite(STEPPER1_PIN_2, LOW);
      digitalWrite(STEPPER1_PIN_3, HIGH);
      digitalWrite(STEPPER1_PIN_4, LOW);
      break;
    case 2:
      digitalWrite(STEPPER1_PIN_1, LOW);
      digitalWrite(STEPPER1_PIN_2, HIGH);
      digitalWrite(STEPPER1_PIN_3, LOW);
      digitalWrite(STEPPER1_PIN_4, LOW);
      break;
    case 3:
      digitalWrite(STEPPER1_PIN_1, HIGH);
      digitalWrite(STEPPER1_PIN_2, LOW);
      digitalWrite(STEPPER1_PIN_3, LOW);
      digitalWrite(STEPPER1_PIN_4, LOW);
      break;
  }
}
