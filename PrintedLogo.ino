// pins
#define LED          10
#define IR_SENSOR     5
#define RelOut        4
// set var
#define MAX_DISTANCE 30
#define DELTA_CM     1.4  // 0.45 0.5 0.8
#define distMeasurementsAmmount 5

float zeroPointCm = 0.0;
bool state_LED = true;
float CurrentDistCm = 0.0;

bool flag = false;


float IRAverageDist(int ammount = distMeasurementsAmmount) {
	float res = 0;
	for (int i = 0; i<ammount; i++) {
		delay(1);
		int k = analogRead(IR_SENSOR);
		res = res + k;
	}
	res = round(res / ammount);
	float volts = res*0.0048828125;  // value from sensor * (5/1024)
	float double_accuracy = 13*pow(volts, -1); // worked out from datasheet graph
	if (double_accuracy > MAX_DISTANCE)
	{
		double_accuracy = MAX_DISTANCE;
	}
	return double_accuracy;
}

float initProcedure() {
	float avarageDist = 0;

	for(int i = 0; i < 4; i++)
	{
		avarageDist += IRAverageDist();
		digitalWrite(LED, HIGH);
		delay(500);
		digitalWrite(LED, LOW);
	}
	avarageDist = avarageDist/4.0;
	Serial.println("init is done!");
	return avarageDist;
}

bool soleChecker(float zeroPoint, float IRAverDist, float delta) {

	if(zeroPoint - IRAverDist > delta)
	{
		return true;
	}
	return false;
}

void setup() {

  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(RelOut, OUTPUT);

  while(zeroPointCm == 30 || zeroPointCm < 5)
  {
  	zeroPointCm = initProcedure(); // init
  }
  //zeroPointCm = 17.2;
}

void loop() {
	//Serial.println(IRAverageDist());
	Serial.println("Debug point 1");
	if(soleChecker(zeroPointCm, IRAverageDist(), DELTA_CM))
	{
		Serial.println("Debug point 2");
		if(state_LED != true)
		{
			Serial.println("Debug point 3");
			int tmp = 0;  // if random value error counter
			// error here ???
			for(int i = 0; i < 8; i++) // 10 * delay(50) = 500 wait after someone putted suole
			{
				Serial.println("Debug point 4");
				CurrentDistCm = IRAverageDist();
				Serial.println("Debug point 5");
				delay(50);
				if(abs(CurrentDistCm - IRAverageDist()) < DELTA_CM)
				//if(abs(zeroPointCm - IRAverageDist()) < DELTA_CM)
				{
					Serial.println("Debug point 6");
					flag = true;
				}
				else
				{
					Serial.println("Debug point 7");
					// flag = false;
					// Serial.println("flag = false!!!");
					tmp++;
					// break; // перевірити тут
				}
				// set up this counter
				if(tmp >= 2)  // if ammount of random value error > 2 
				{
					Serial.println("Debug point 8");
					flag = false;
					Serial.println("flag = false...");
					break;
				}
			}
			Serial.println("Debug point 9");
			if(flag)
			{
				Serial.println("Debug point 10");
				digitalWrite(RelOut, HIGH); // спрацювання клапана
				Serial.println("(if) RelOut HIGH");
				delay(200); // 200 ms працює клапан
				Serial.println("Debug point 11");
				digitalWrite(RelOut, LOW);
				Serial.println("(if) RelOut LOW");
				state_LED = true; // ??? why we need it?
				Serial.println("Debug point 12");
				delay(3000); // затримка від хибних спрацювань під час відпрацювання циклу машини (цикл машини 3,5 с)
				Serial.println("Debug point 13");
			}
		}
	}
	else
	{
		Serial.println("Debug point 14");
		digitalWrite(RelOut, LOW);
		Serial.println("(else) RelOut LOW");
		Serial.println("Debug point 15");
		state_LED = false;
		Serial.println("Debug point 16");
	}
	Serial.println("Debug point 17");
}
