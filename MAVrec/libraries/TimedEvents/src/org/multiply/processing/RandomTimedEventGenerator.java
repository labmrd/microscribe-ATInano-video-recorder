/**
 * TimedEvents
 * A couple of classes for firing off timed events at regular or random intervals.
 * http://multiply.org/processing/
 *
 * Copyright 2012 Jason Gessner http://multiply.org/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author      Jason Gessner http://multiply.org/
 * @modified    05/18/2013
 * @version     1.0.1 (2)
 *
 * This is a simple library that will invoke a method you provide
 * in your sketch periodically at a random time between {@code minIntervalMs} and 
 * {@code maxIntervalMs} milliseconds.
 * 
 * You may change the interval range using {@code setMinIntervalMs} and {@code setMaxIntervalMs}
 * and enable or disable the {@code RandomTimedEventGenerator} using {@code setIsEnabled}.
 */
package org.multiply.processing;

import java.lang.reflect.Method;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;

import processing.core.*;

public class RandomTimedEventGenerator {
  private class MethodInvoker implements Runnable {
    @Override
    public void run() {
      try {
        timerEvent.invoke(parentApplet);
        // schedule the next invocation
        scheduleTask();
      } catch (Exception e) {
        System.err.println("Disabling " + timerEventName + "() for RandomTimedEventGenerator " +
            "because of an error.");
        e.printStackTrace();
        cancelTask();
      }
    }
  }

  public final static String VERSION = "1.0";
  private static final String defaultEventName = "onRandomTimerEvent";
  private static final int defaultMinIntervalMs = 500;
  private static final int defaultMaxIntervalMs = 2000;

  private PApplet parentApplet;
  private ScheduledExecutorService executor;
  private String timerEventName;
  private Method timerEvent;
  private int minIntervalMs;
  private int maxIntervalMs;
  private boolean isEnabled;
  private ScheduledFuture<?> currentTask;

  /**
   * Construct an enabled RandomTimedEventGenerator with the default event handler name of
   * <code>onTimerEvent</code> and the default interval range of 500ms - 2000ms.
   *
   * @param parentApplet the parent Applet that this library runs inside (this in your sketch).
   */
  public RandomTimedEventGenerator(PApplet parentApplet) {
    this(parentApplet, defaultEventName, true);
  }

  /**
   * Construct an enabled RandomTimedEventGenerator with a custom event handler name and the default
   * interval range of 500ms - 2000ms.
   *
   * @param parentApplet the parent Applet that this library runs inside (this in your sketch).
   * @param timerEventName the name of the method to invoke when the timer fires.
   */
  public RandomTimedEventGenerator(PApplet parentApplet, String timerEventName) {
    this(parentApplet, timerEventName, true);
  }
  
  /**
   * Construct a RandomTimedEventGenerator with a specific event handler name and specific enabled
   * status.
   *
   * @param parentApplet the parent Applet that this library runs inside (this in your sketch).
   * @param timerEventName the name of the method to invoke when the timer fires.
   * @param isEnabled determines if the RandomTimedEventGenerator starts off active or not.
   */
  public RandomTimedEventGenerator(PApplet parentApplet, String timerEventName, boolean isEnabled) {
    this(parentApplet, timerEventName, isEnabled, defaultMinIntervalMs, defaultMaxIntervalMs);
  }
  
  /**
   * Construct a RandomTimedEventGenerator with a specific event handler name, a specific enabled
   * status and a specific interval.
   *
   * @param parentApplet the parent Applet that this library runs inside (this in your sketch).
   * @param timerEventName the name of the method to invoke when the timer fires.
   * @param isEnabled determines if the RandomTimedEventGenerator starts off enabled or not.
   */
  public RandomTimedEventGenerator(PApplet parentApplet, String timerEventName, boolean isEnabled,
      int minIntervalMs, int maxIntervalMs) {
    this.parentApplet = parentApplet;
    this.timerEventName = timerEventName;
    this.isEnabled = isEnabled;
    this.minIntervalMs = minIntervalMs;
    this.maxIntervalMs = maxIntervalMs;
    try {
      timerEvent = parentApplet.getClass().getMethod(timerEventName, (Class<?>[]) null);
    } catch (Exception e) {
      this.isEnabled = false;
      throw new RuntimeException("Your sketch is using the RandomTimedEventGenerator without " +
         "defining the target method " + timerEventName + "()!");
    }
    executor = Executors.newSingleThreadScheduledExecutor();
    if (isEnabled) {
      scheduleTask();
    }
  }

  /**
   * @return if the timer events will fire in the parent sketch.
   */
  public boolean isEnabled() {
    return isEnabled;
  }

  /**
   * Turns the timer on or off according to the <code>isEnabled</code> argument.
   *
   * @param isEnabled whether or not the timer event will fire in the parent sketch.
   */
  public void setEnabled(boolean isEnabled) {
    if (this.isEnabled) {
      cancelTask();
    } else {
      scheduleTask();
    }
    this.isEnabled = isEnabled;
  }

  /**
   * Returns the currently configured minimum number of milliseconds between timer events firing.
   *
   * @return the minimum interval time, in milliseconds.
   */
  public int getMinIntervalMs() {
    return minIntervalMs;
  }

  /**
   * Set the number of milliseconds between timer events firing.  Changing this will cancel an
   * enabled timer and immediately schedule a new one.
   *
   * @param minIntervalMs the minimum interval time, in milliseconds.
   */
  public void setMinIntervalMs(int minIntervalMs) {
    this.minIntervalMs = minIntervalMs;
    if (isEnabled) {
      cancelTask();
      scheduleTask();
    }
  }

  /**
   * Returns the currently configured maximum number of milliseconds between timer events firing.
   *
   * @return the maximum interval time, in milliseconds.
   */
  public int getMaxIntervalMs() {
    return maxIntervalMs;
  }

  /**
   * Set the number of milliseconds between timer events firing.  Changing this will cancel an
   * enabled timer and immediately schedule a new one.
   *
   * @param maxIntervalMs the maximum interval time, in milliseconds.
   */
  public void setMaxIntervalMs(int maxIntervalMs) {
    this.maxIntervalMs = maxIntervalMs;
    if (isEnabled) {
      cancelTask();
      scheduleTask();
    }
  }

  /**
   * return the version of the library.
   *
   * @return String
   */
  public static String version() {
    return VERSION;
  }

  private int getNextInvocationDelay() {
    return minIntervalMs + (int) (Math.random() * (maxIntervalMs - minIntervalMs));  
  }
  
  private void scheduleTask() {
    currentTask = executor.schedule(new MethodInvoker(), getNextInvocationDelay(),
        TimeUnit.MILLISECONDS);
  }

  private void cancelTask() {
    if (currentTask != null) {
      currentTask.cancel(true);
    }
  }
}
