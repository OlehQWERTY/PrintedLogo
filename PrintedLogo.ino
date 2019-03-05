// pins
#define LED          10
#define IR_SENSOR     5
#define RelOut        4
// set var
#define MAX_DISTANCE 30
#define DELTA_CM     1.5  // 0.45 0.5 0.8
#define distMeasurementsAmmount 5

float zeroPointCm = 0.0;
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
	Serial.println("Init procedure is done!");
	return avarageDist;
}

bool soleChecker(float zeroPoint = zeroPointCm, float IRAverDist = IRAverageDist(), float delta = DELTA_CM) {
	if( IRAverDist > zeroPoint && (IRAverDist - zeroPointCm) > DELTA_CM ) // (zeroPoint - IRAverDist > delta)
	{
		return true;
	}
	return false;
}

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(RelOut, OUTPUT);

  while(zeroPointCm < 5)
  {
  	zeroPointCm = initProcedure(); // init
  }
}

void loop() {
	Serial.println(IRAverageDist());
	if(soleChecker())
	{
		flag = false;
		int tmp = 1;  // random value error counter
		for(int i = 0; i < 8; i++) // 8 * delay(50) = 400 wait after someone putted suole
		{
			CurrentDistCm = IRAverageDist();
			Serial.println(CurrentDistCm);
			delay(50);

			if(soleChecker())
			{
				flag = true;
			}
			else
			{
				tmp++;
			}
			if(tmp >= 4)  // if ammount of random error >= number
			{
				flag = false;
				Serial.println("flag = false...");
				break;
			}
		}
	}
	
	if(flag)
	{
		digitalWrite(RelOut, HIGH); // спрацювання клапана
		Serial.println("(if) RelOut HIGH");
		delay(200); // 200 ms працює клапан
		digitalWrite(RelOut, LOW);
		Serial.println("(if) RelOut LOW");
		delay(3500); // затримка від хибних спрацювань під час відпрацювання циклу машини (цикл машини 3,5 с)
	}
	else
	{
		digitalWrite(RelOut, LOW);
		// Serial.println("(else) RelOut LOW");
	}
}
