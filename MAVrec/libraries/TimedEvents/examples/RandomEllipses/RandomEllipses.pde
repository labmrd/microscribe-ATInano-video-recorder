import org.multiply.processing.RandomTimedEventGenerator;

private RandomTimedEventGenerator randomEvent;
private RandomTimedEventGenerator awesomeRandomEvent;
private int lastMillis = 0;
private int lastAwesomeMillis = 0;

void setup() {
  size(400, 400);
  randomEvent = new RandomTimedEventGenerator(this);
  randomEvent.setMinIntervalMs(11);
  randomEvent.setMaxIntervalMs(999);
  awesomeRandomEvent = new RandomTimedEventGenerator(this, "myAwesomeRandomEvent");
  awesomeRandomEvent.setMinIntervalMs(1500);
  awesomeRandomEvent.setMaxIntervalMs(9999);
  
  ellipseMode(CENTER);
  smooth();
}

void draw() {
  // everything in this sketch is driven by timer events from the
  // RandomTimerEvenGenerator objects.
}

void setRandomFillAndStroke() {
  stroke(color(random(255), random(255), random(255)));
  fill(color(random(255), random(255), random(255)));
  strokeWeight(random(13));
}

void onRandomTimerEvent() {
  int millisDiff = millis() - lastMillis;
  lastMillis = millisDiff + lastMillis;  
  System.out.println("Got a random event at " + millis() + "ms (" + millisDiff + ")!");
  setRandomFillAndStroke();
  ellipse(random(width), random(height), random(100), random(100));
}

void myAwesomeRandomEvent() {
  int millisDiff = millis() - lastAwesomeMillis;
  lastAwesomeMillis = millisDiff + lastAwesomeMillis;  
  System.out.println("Got a specifically awesome random event at " + millis() +
      "ms (" + millisDiff + ")!");
  background(color(random(255), random(255), random(255)));
}
