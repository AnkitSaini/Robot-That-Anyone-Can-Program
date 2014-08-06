/*Following Bytes are sent when a button is pressed :---                        
         Forward(1) ;                                                            
         Back(2) : 
         TurnLeft(3) ;  
         TurnRight(4) ; 
         Stop(5) ; 
         ReturnButton(6) : 
         RunWithPreviousConfigButton(7); 
         ProgramButton(8) ;
         ManualControl(9) ;
       **Screen2 ToBeDeleted_StopTemp(11);  
*/

/* Size of differen data types in Arduino
           sizeof(int)=2
           sizeof(long)=4
           sizeof(long long)=8
           sizeof(int8_t)=1
           sizeof(int16_t)=2
           sizeof(int32_t)=4
           sizeof(int64_t)=8
           sizeof(uint8_t)=1
           sizeof(uint16_t)=2
           sizeof(uint32_t)=4
           sizeof(uint64_t)=8
*/
 
#include <SoftwareSerial.h>//Software Serial Port
#include <EEPROMEx.h>
#define RxD 6
#define TxD 7

const byte motorpin3  =  3;                  //define digital output pin no.
const byte motorpin5  =  5;                  //define digital output pin no.
const byte motorpin9  =  9;                  //define digital output pin no.
const byte motorpin10 =  10;                  //define digital output pin no.
const byte ledPin     = 13; 


const int bDirectionAddress = 0xA0 ;        // location( address 10 ) where the direction is stored in EEPROM , 32 bits are used
const int bMagnitudeAddress = 0x4B ;        // location( address 75 ) where magnitude is stored in EEPROM ( address 75 ) , immediately after direction , direction is stored till address 74 ( 10 + 64 bits)         

uint32_t numberConstructed[] = {0, 0} ;      // to store the directions 
byte iii = 0 , jjj = 0;                      // counters to keep track of how many numbers are stored
                                             // iii keeps track of how many numbers are stored in a 4 byte element of numberConstructed[] , and its value goes from 0 to 9 as the biggest
                                             // 4 byte number has 10 digits
                                             // jjj keeps track of the element of numberConstructed[] being used

uint32_t magnitudeNumber[] = { 0, 0};                  // to store the magnitude with each direction
byte magnitudeCounter = 0 , magnitudeArrayElement = 0; // counters to keep track of how many numbers are stored

SoftwareSerial blueToothSerial(RxD,TxD);

void BlinkLed()
{
  digitalWrite( ledPin , HIGH);
  delay(500);
  digitalWrite( ledPin , LOW);
  delay(100);
}
uint32_t Power(int base , int power )  // Pow() function that is available with arduino returns floating point value , and as we cannot use modulus operator on floating point values
{                                      // hence this function is written ,and we require a 4 byte number hence its return type is uint32_t
  uint32_t answer = 1; 
  for(int i = 0 ; i < power ; i++)
  {
    answer = answer*base;
  }
  return answer;
}

unsigned long MakeDirectionNumber(unsigned long a , int y) // to make direction number , 
{
    a = a*10 + y;
   return a; 
}

void SeparateDirectionNumber(unsigned long a) // to separate direction number 
{
    while( a!=0)
    {
    int y ;
    y = a%10;
    if ( a/10 == 0)
    { y = a;}
     a = a/10;
    Serial.println(y);
    }
} 

unsigned long MakeMagnitudeNumber(unsigned long b , int z) // where b is the magnitudeNumber and z is the magnitude in a direction
{
  b = b*100 + z;
  return b;
}

void SeparateMagnitudeNumber(unsigned long b)
{
  while(b != 0)
  {
    int y;
    y = b%100;
    if( b/100 == 0) // Suppose b = 1420  and then b = 14 , b/100 = 0 hence in the above statement y = b%100 , y = 0; thus to print the last number that is 14 this if statement is required
    { y = b;}
    b = b/100;
    Serial.println(y);
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------
byte CheckSentValue(byte nValue) // To recieve and return the value sent from remote(Android) Bluetooth shield
{
  Serial.println(nValue);
  if( blueToothSerial.read() == 5 )
  {
    nValue = 5;
    return nValue;
  }
  else
  {
    return nValue;
  }
  Serial.println(nValue);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ForwardMovement(byte &bSentMovementValue , byte bStoredMagnitude = 0) // To move the bot forward
{
  byte bMagnitude = 0;
  
  if( bStoredMagnitude != 0)  //this if loop is executed when running the bot on the path stored in memory
  {
    bMagnitude = bStoredMagnitude;
    digitalWrite(motorpin3,LOW);
    digitalWrite(motorpin5,HIGH);
    
    digitalWrite(motorpin9,LOW);
    digitalWrite(motorpin10,HIGH);
    
    while( bMagnitude != 0)
    {
      bMagnitude--;
      delay(100);
    }
    Stop();
    bSentMovementValue = 5;
  }
  else
  {
    digitalWrite(motorpin3,LOW);
    digitalWrite(motorpin5,HIGH);
    
    digitalWrite(motorpin9,LOW);
    digitalWrite(motorpin10,HIGH);
    
    while(bSentMovementValue == 1)
    {
      bMagnitude ++ ;
      bSentMovementValue = CheckSentValue(bSentMovementValue);
      delay(100);
      if( bMagnitude == 99 )
      {
        Stop();
        BlinkLed();
        bSentMovementValue = 5 ;
      } 
        
    }
    Stop();
    if(magnitudeCounter == 0 && bMagnitude >= 42) // this if loop is written because:- biggest number that can be stored in 4 bytes is 4294967295
    {                                             // hence while storing the first two number , if the number is greater than or equal to 42 , and we make a full 10 digit 4 byte number
      iii++;                                      // while storing the last two numbers the whole 4 byte numbr will go out of scope ,and a random number will be made, hence in order to 
      magnitudeCounter++;                         // prevent this we only store 8 numbers and hence we increase the magnitudeCounter by 1 in this if loop.
    }
    magnitudeNumber[magnitudeArrayElement] = MakeMagnitudeNumber( magnitudeNumber[magnitudeArrayElement] , bMagnitude);
  }

}


void BackwardMovement(byte &bSentMovementValue , byte bStoredMagnitude = 0) //**** To move the bot backward*****
{
  byte bMagnitude = 0;
  if( bStoredMagnitude != 0)  //this if loop is executed when running the bot on the path stored in memory
  {
    bMagnitude = bStoredMagnitude;
    digitalWrite(motorpin3,HIGH);
    digitalWrite(motorpin5,LOW);
    
    digitalWrite(motorpin9,HIGH);
    digitalWrite(motorpin10,LOW);
    while( bMagnitude != 0)
    {
      bMagnitude --;
      delay(100);
    }
    Stop();
    bSentMovementValue = 5;
  }
  else
  {
    digitalWrite(motorpin3,HIGH);
    digitalWrite(motorpin5,LOW);
    
    digitalWrite(motorpin9,HIGH);
    digitalWrite(motorpin10,LOW);
    while(bSentMovementValue == 2)
    {
     bMagnitude++;
     bSentMovementValue = CheckSentValue(bSentMovementValue);
     delay(100);
     if( bMagnitude == 99 )
      {
        Stop();
        BlinkLed();
        bSentMovementValue = 5 ;
      }
    }
    Stop();
    if(magnitudeCounter == 0 && bMagnitude >= 42)
    {
     iii++;
     magnitudeCounter++;
    }
    magnitudeNumber[magnitudeArrayElement] = MakeMagnitudeNumber( magnitudeNumber[magnitudeArrayElement] , bMagnitude);
  }
}


void TurnLeft(byte &bSentMovementValue , byte bStoredMagnitude = 0) //*** To turn the bot left***************
{
  byte bMagnitude = 0;
  if( bStoredMagnitude != 0)  //this if loop is executed when running the bot on the path stored in memory
  {
    bMagnitude = bStoredMagnitude;
    while(bMagnitude != 0)
    {
      digitalWrite(motorpin3,HIGH);
      digitalWrite(motorpin5,LOW);
    
      digitalWrite(motorpin9,LOW);
      digitalWrite(motorpin10,HIGH);
      delay(1850);
      Stop();
      bMagnitude--;
    }
    //Stop();
    bSentMovementValue = 5;
  }
  else
  {
    while( bSentMovementValue == 3)
    {
      digitalWrite(motorpin3,HIGH);
      digitalWrite(motorpin5,LOW);
    
      digitalWrite(motorpin9,LOW);
      digitalWrite(motorpin10,HIGH);
      delay(1950);
      Stop();
      bMagnitude++;
      delay(50);
      for( int t = 0 ; t < 600 ; t ++ )
      { 
         bSentMovementValue = CheckSentValue(bSentMovementValue);
      }
    }
    /*while(bSentMovementValue == 3)
    {
     bMagnitude++;
     bSentMovementValue = CheckSentValue(bSentMovementValue);
     delay(100);
    }
    Stop();*/
    if(magnitudeCounter == 0 && bMagnitude >= 42)
    {
     iii++;
     magnitudeCounter++;
    }
    magnitudeNumber[magnitudeArrayElement] = MakeMagnitudeNumber( magnitudeNumber[magnitudeArrayElement] , bMagnitude);
  }
}

void TurnRight(byte &bSentMovementValue , byte bStoredMagnitude = 0) //******* To turn the bot right*************
{
  byte bMagnitude = 0;
  if( bStoredMagnitude != 0)  //this if loop is executed when running the bot on the path stored in memory
  {
    bMagnitude = bStoredMagnitude;
    while( bMagnitude != 0 )
    {
      digitalWrite(motorpin3,LOW);
      digitalWrite(motorpin5,HIGH);
    
      digitalWrite(motorpin9,HIGH);
      digitalWrite(motorpin10,LOW);
      delay(1850);
      Stop();
      bMagnitude--;
      delay(50);
    }
    //Stop();
    bSentMovementValue = 5;
  }
  else
  {
    while(bSentMovementValue == 4)
    {
      digitalWrite(motorpin3,LOW);
      digitalWrite(motorpin5,HIGH);
    
      digitalWrite(motorpin9,HIGH);
      digitalWrite(motorpin10,LOW);
      delay(1950);
      Stop();
      bMagnitude++;
      for(int t = 0 ; t < 600 ; t ++ )
      {
        bSentMovementValue = CheckSentValue(bSentMovementValue);
      }
    }  
    /*while(bSentMovementValue == 4)
    {
     bMagnitude++ ;
     bSentMovementValue = CheckSentValue(bSentMovementValue);
     delay(100);
    }
    Stop();*/
    if(magnitudeCounter == 0 && bMagnitude >= 42)
    {
     iii++;
     magnitudeCounter++;
    }
    magnitudeNumber[magnitudeArrayElement] = MakeMagnitudeNumber( magnitudeNumber[magnitudeArrayElement] , bMagnitude);
  }
}

void Stop()
{
  digitalWrite(motorpin3,LOW);
  digitalWrite(motorpin5,LOW);
  digitalWrite(motorpin9, LOW);
  digitalWrite(motorpin10,LOW);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------- 

void Direction(byte &bSentMovementValue , byte bStoredMagnitude = 0)
{
       if(bSentMovementValue == 1)
       {
         if(bStoredMagnitude != 0)   //this if loop is executed when running the bot on the path stored in memory
         {ForwardMovement(bSentMovementValue , bStoredMagnitude);}
         else
         {ForwardMovement(bSentMovementValue);}
       }
       if (bSentMovementValue == 2)
       {
         if(bStoredMagnitude != 0)   //this if loop is executed when running the bot on the path stored in memory
         {BackwardMovement(bSentMovementValue , bStoredMagnitude);}
         else
         {BackwardMovement(bSentMovementValue);}
       }
       if(bSentMovementValue == 3)
       {
         if(bStoredMagnitude != 0)   //this if loop is executed when running the bot on the path stored in memory
         {TurnLeft(bSentMovementValue , bStoredMagnitude);}
         else
         { TurnLeft(bSentMovementValue);}
       }
       if(bSentMovementValue == 4)
       {
         if(bStoredMagnitude != 0)   //this if loop is executed when running the bot on the path stored in memory
         {TurnRight(bSentMovementValue , bStoredMagnitude);}
         else
         { TurnRight(bSentMovementValue);}
       }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Here "a" is the "numberConstructed" and "s" is the "magnitudeNumber"
void ReturnPath(uint32_t a , byte bDirectionArrayElement, uint32_t *s) // to bring the robot back following the same path as path()
{
  byte bDirectionRemainder = 0; 
  byte bMagnitudeRemainder = 0;
  magnitudeCounter = 0;
  uint32_t b[2] ; // to store the values of the magnitudeNumber ,i.e. s here , this is because as the entire magnitude number array is passed hence using it
                  // will change the values of its elements , so we assign its values to b[].
                  // b[] has only two elements because for a given direction number we only require 2 magnitude numbers.
  
  byte magnitudeArrayElementLocal = 0; // to access the values of b[]
  byte bNumberofDirections = 0;
  uint32_t bbb = a;
  while( bbb != 0)
  {
    bbb=bbb/10;
    bNumberofDirections++;
  }
  if( bNumberofDirections > 5 )
  { magnitudeCounter = bNumberofDirections - 5 ;
     magnitudeArrayElement = bDirectionArrayElement*2+1; 
     magnitudeArrayElementLocal = 1;  
  }
  if( bNumberofDirections < 5 )
  { magnitudeCounter = bNumberofDirections ; 
     magnitudeArrayElement = bDirectionArrayElement*2;
     magnitudeArrayElementLocal = 0;
  } 
  
  for( byte iii = 0 ; iii < 2 ; iii++ )
  {
    b[iii] = s[iii];
  }
  
  if( b[0] >= 100000000)
  {
    magnitudeCounter = magnitudeCounter - 1;
  }
     
  while( a != 0 )
  {
    bDirectionRemainder = a%10;
    bMagnitudeRemainder = b[magnitudeArrayElementLocal]%100;
     if( b[magnitudeArrayElementLocal]/100 == 0 )
     { 
       bMagnitudeRemainder = b[magnitudeArrayElementLocal];
     }
     if( a/10 == 0 )
     { bDirectionRemainder = a; }
     
     if(bDirectionRemainder == 3 || bDirectionRemainder == 4)
     {
       if(bDirectionRemainder == 3)
       {bDirectionRemainder = 4;}
       else
       {bDirectionRemainder = 3;}
     }
     
     Direction( bDirectionRemainder , bMagnitudeRemainder);
     a = a/10;
     b[magnitudeArrayElementLocal] = b[magnitudeArrayElementLocal]/100;
     // This function is after the division so that when the b[0] is to be evaluated after b[1] , the last 5th magnitude can be taken
     // if this function is placed before the division the 5th magnitude of b[0] will be last and the 1st direction i.e. a[0] will go in loop as the magnitude for that direction will be 0
     // For example:-- if only a[0] is used
     //if b[1] = 5050709090 and b[0] = 4011060509 , a[0] = 1234123416 thus if the function is placed before division 09 of b[0] will be lost and 1 of a[0] will set in an loop
     if( magnitudeCounter == 0 && magnitudeArrayElementLocal== 1 ) 
     {
       magnitudeCounter = 5; //this value is made 5 because in the last line of this while( a != 0) loop it will decrease by 1 from 5 to 0
       magnitudeArrayElementLocal-- ;
     }
     magnitudeCounter-- ; 
     
   }
} 
    


void Path(uint32_t a , byte bDirectionArrayElement, uint32_t *s)
{
  byte bDirection = 0;  
  byte bMagnitude = 0;
  byte bPowerMagElementLocal0 = 0;
  byte bPowerMagElementLocal1 = 0;
  byte bPowerDirection = 0;
  byte bPowerMagnitude = 0;
  magnitudeCounter = 0;
  uint32_t b[2] ; // to store the values of the magnitudeNumber ,i.e. s here , this is because as the entire magnitude number array is passed hence using it
                  // will change the values of its elements , so we assign its values to b[].
                  // b[] has only two elements because for a given direction number we only require 2 magnitude numbers.
  
  byte magnitudeArrayElementLocal = 0; // to access the values of b[]
  byte bNumberofDirections = 0;
  uint32_t bbb = a;
  while( bbb != 0)
  {
    bbb=bbb/10;
    bNumberofDirections++;
  }
 
  if( bNumberofDirections > 5 )
  { 
    magnitudeCounter = bNumberofDirections - 5 ;
  }
  if( bNumberofDirections < 5 )
  { 
    magnitudeCounter = bNumberofDirections ; 
  } 
  magnitudeArrayElement = bDirectionArrayElement*2;
  for( byte iii = 0 ; iii < 2 ; iii++ )
  {
    b[iii] = s[magnitudeArrayElement];
    magnitudeArrayElement++;
  }
  magnitudeArrayElement = bDirectionArrayElement*2;
  if(b[1] == 0)
  {
    bPowerMagElementLocal0 = bNumberofDirections - 1 ;
    bPowerMagElementLocal1 = 0;
  }
  
  if( b[1] != 0 )
  {
    if( b[0] >= 100000000 && bNumberofDirections > 5 )
    {
      bPowerMagElementLocal0 = 4;
      bPowerMagElementLocal1 =( bNumberofDirections - 5 ) - 1;
    }
    if( b[0] < 100000000 && bNumberofDirections >= 5)
    {
      bPowerMagElementLocal0 = 3 ;
      bPowerMagElementLocal1 = ( bNumberofDirections - 5);
    }
  }
   
  bPowerDirection = bNumberofDirections - 1;
  bPowerMagnitude = bPowerMagElementLocal0;
  
  while( a!= 0)
  {
    bDirection = a/Power(10,bPowerDirection);
    bMagnitude = b[magnitudeArrayElementLocal]/Power(10,(bPowerMagnitude*2) );
    
    if(a/Power(10,bPowerDirection)==0)
    { bDirection = a;}
    if(b[magnitudeArrayElementLocal]/Power(10,(bPowerMagnitude*2) )==0)
    { bMagnitude = b[magnitudeArrayElementLocal]; }
    
    if( bDirection == 6 )
    {
      bDirection = 4 ;
      bMagnitude = 2 ;
    }
    Direction( bDirection , bMagnitude );
    
    a = a%(Power( 10 , bPowerDirection ));
    b[magnitudeArrayElementLocal] = b[magnitudeArrayElementLocal]%Power(10,(bPowerMagnitude*2));
    
    
    if(bPowerMagnitude == 0)
    {
      bPowerMagnitude = bPowerMagElementLocal1 + 1 ; // one is added because bPowerMagnitude will decrease by one as this if block ends and hence a value will be lost in the end
      magnitudeArrayElementLocal = 1 ;
    }
    bPowerDirection = bPowerDirection - 1;
    bPowerMagnitude = bPowerMagnitude - 1;
    
  }
     

}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup() 
{ 
  Serial.begin(9600);
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  
  pinMode(motorpin3,OUTPUT);        //set pin 3 as output
  pinMode(motorpin5,OUTPUT);        // set pin 5 as output
  pinMode(motorpin9,OUTPUT);        //set pin 9 as output
  pinMode(motorpin10,OUTPUT);       //set pin 10 as output
  pinMode(ledPin , OUTPUT);
  setupBlueToothConnection(); 
} 
 
void loop() 
{ 
  //Serial.println(numberConstructed);
  byte recievedByte;    // to store value of recieved direction 
  byte bControlValue=0; // to recieve value to check for manual control or programming robot control 
  while(1)
  {
    if(blueToothSerial.available())
    {bControlValue = blueToothSerial.read();}
    
    //*******This is Manual Control Section**************************************************************************
    while(bControlValue == 9)
    {
      while(blueToothSerial.available())
      {
        recievedByte = blueToothSerial.read();
        
        if(recievedByte == 11)
        {bControlValue = 11;}
        else
        {
          Direction(recievedByte);
        }
        }
    }
    //***************Manual control section ends***********************************************************************
    
    //******************Program Robot section****************************************************************************
    while(bControlValue == 8)
    {
      while(blueToothSerial.available())
      {
        recievedByte = blueToothSerial.read();
        if( recievedByte != 11 )
        {
          if( recievedByte !=5 && recievedByte != 6 && recievedByte < 8 )
          {
            if( iii < 10 && jjj < 1) // iii goes from 0 to 9 to store 10 directions in array member
            {              // jjj is 0 for first member and 1 for second member , and is less than 2 because we have only two numberConstructed array members
              numberConstructed[jjj] = MakeDirectionNumber(numberConstructed[jjj] , recievedByte);
              Direction(recievedByte);
              iii = iii + 1;
              magnitudeCounter = magnitudeCounter + 1;
              if(magnitudeCounter == 5)
              {
                magnitudeCounter = 0;
                magnitudeArrayElement = magnitudeArrayElement + 1;
              }
              
              if ( iii == 9 && jjj == 0)
              {
                numberConstructed[jjj] = MakeDirectionNumber(numberConstructed[jjj] , 6);
                magnitudeNumber[magnitudeArrayElement] = MakeMagnitudeNumber(magnitudeNumber[magnitudeArrayElement] , 0 );
                BlinkLed();
                BlinkLed();
                
                EEPROM.updateBlock( bDirectionAddress , numberConstructed , 2 );
                EEPROM.updateBlock( bMagnitudeAddress , magnitudeNumber , 2 );
               
                recievedByte = 3 ;
                TurnLeft( recievedByte , 2 );
                
                for( jjj = 0 ; jjj < 255 ; jjj -- )
                {
                  ReturnPath( numberConstructed[jjj] , jjj , magnitudeNumber);
                }
                iii ++ ;
              }
              if( iii == 10)
              {
                iii = 0;
                jjj = jjj + 1;
              }
            }
            if( jjj > 0)
            {
              BlinkLed();
            }
          }
          
          if(recievedByte == 6 )
          {
            if(jjj == 0)
            {
              recievedByte = 3 ;
              TurnLeft( recievedByte , 2);
              recievedByte = 6;
              numberConstructed[jjj] = MakeDirectionNumber(numberConstructed[jjj] , recievedByte);
              magnitudeNumber[magnitudeArrayElement] = MakeMagnitudeNumber(magnitudeNumber[magnitudeArrayElement] , 0);
              
              EEPROM.updateBlock( bDirectionAddress , numberConstructed , 2 );
              EEPROM.updateBlock( bMagnitudeAddress , magnitudeNumber , 2 );
            }
            
            for( jjj = 0 ; jjj < 255 ; jjj -- )
            {
              ReturnPath( numberConstructed[jjj] , jjj , magnitudeNumber);
            }
            
            recievedByte = 3;
            TurnLeft( recievedByte , 2 );
            recievedByte = 6;
          }
        }
        
        if(recievedByte == 11 )
        {
          bControlValue == 11 ;
        }
      }
    }
    /*********************Program Robot Section Ends Here********************************************/
    
    /************** Code to run Robot on stored Path***********************************************/
    while( bControlValue == 7 )
    {
      EEPROM.readBlock( bDirectionAddress , numberConstructed , 2 );
      EEPROM.readBlock( bMagnitudeAddress , magnitudeNumber , 2 );
      
      for( jjj = 0 ; jjj < 255 ; jjj -- )
      {
        Path( numberConstructed[jjj] , jjj , magnitudeNumber );
       }
       
       for( jjj = 0 ; jjj < 255 ; jjj -- )
       {
         ReturnPath( numberConstructed[jjj] , jjj , magnitudeNumber);
       }
       
       recievedByte = 3;
       TurnLeft( recievedByte , 2 );
       recievedByte = 7;
              
       for( int t = 0 ; t < 600 ; t ++ )
       {
         bControlValue = CheckSentValue( bControlValue );
       }
     }
   }
}
 
 
void setupBlueToothConnection()
{
  blueToothSerial.begin(38400); //Set BluetoothBee BaudRate to default baud rate 38400
  blueToothSerial.print("\r\n+STWMOD=0\r\n"); //set the bluetooth work in slave mode
  blueToothSerial.print("\r\n+STNA=SeeedBTSlave\r\n"); //set the bluetooth name as "SeeedBTSlave"
  blueToothSerial.print("\r\n+STPIN=0000\r\n");//Set SLAVE pincode"0000"
  blueToothSerial.print("\r\n+STOAUT=1\r\n"); // Permit Paired device to connect me
  blueToothSerial.print("\r\n+STAUTO=0\r\n"); // Auto-connection should be forbidden here
  delay(2000); // This delay is required.
  blueToothSerial.print("\r\n+INQ=1\r\n"); //make the slave bluetooth inquirable 
  Serial.println("The slave bluetooth is inquirable!");
  delay(2000); // This delay is required.
  blueToothSerial.flush();
}




