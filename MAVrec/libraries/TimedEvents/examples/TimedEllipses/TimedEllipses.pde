import org.multiply.processing.TimedEventGenerator;

private TimedEventGenerator ellipseTimedEventGenerator;
private TimedEventGenerator rectangleTimedEventGenerator;

private int lastMillis = 0;

void setup() {
  size(400, 400);
  
  ellipseTimedEventGenerator = new TimedEventGenerator(this);
  ellipseTimedEventGenerator.setIntervalMs(500);
  
  rectangleTimedEventGenerator = new TimedEventGenerator(
      this, "onRectangleTimerEvent", false);
  rectangleTimedEventGenerator.setIntervalMs(175);
  
  ellipseMode(CENTER);
  rectMode(CENTER);
  smooth();
}

void draw() {
  // everything in this sketch is driven by timer events from the
  // TimerEvenGenerator objects.
}

void keyReleased() {
  if (key == 'e') {
    // Turn the ellipseTimedEventGenerator on and off.
    ellipseTimedEventGenerator.setEnabled(
        !ellipseTimedEventGenerator.isEnabled());
  } else if (key == 'E') {
    // Set a new interval randomly selected between 400 and 2000 milliseconds.
    ellipseTimedEventGenerator.setIntervalMs((int) random(400, 2000));
  } else if (key == 'r') {
  // Turn the rectangleTimedEventGenerator on and off.
    rectangleTimedEventGenerator.setEnabled(
        !rectangleTimedEventGenerator.isEnabled());
  } else if (key == 'R') {
    // Set a new interval randomly selected between 400 and 2000 milliseconds.
    rectangleTimedEventGenerator.setIntervalMs((int) random(400, 2000)); 
  }
}

void setRandomFillAndStroke() {
  stroke(color(random(255), random(255), random(255)));
  fill(color(random(255), random(255), random(255)));
  strokeWeight(random(13));
}

void onTimerEvent() {
  int millisDiff = millis() - lastMillis;
  lastMillis = millisDiff + lastMillis;  
  System.out.println("Got a timer event at " + millis() + "ms (" + millisDiff + ")!");
  setRandomFillAndStroke();
  ellipse(random(width), random(height), random(100), random(100));
}

void onRectangleTimerEvent() {
  System.out.println("Got a rectangle timer event!");
  setRandomFillAndStroke();
  rect(random(width), random(height), random(100), random(100));
}
