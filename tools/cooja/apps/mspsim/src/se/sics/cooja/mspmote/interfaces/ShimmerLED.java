package se.sics.cooja.mspmote.interfaces;

import java.awt.*;
import java.util.*;
import javax.swing.*;
import org.apache.log4j.Logger;
import org.jdom.Element;
import se.sics.cooja.*;
import se.sics.cooja.interfaces.LED;
import se.sics.cooja.mspmote.ShimmerMote;
import se.sics.mspsim.core.IOPort;
import se.sics.mspsim.core.IOUnit;
import se.sics.mspsim.core.PortListener;
import se.sics.mspsim.platform.shimmer.ShimmerNode;

@ClassDescription("Shimmer LED")
public class ShimmerLED extends LED {
  private static Logger logger = Logger.getLogger(ShimmerLED.class);

  private ShimmerMote mspMote;
  private boolean blueOn = false;
  private boolean greenOn = false;
  private boolean redOn = false;

  private static final Color DARK_BLUE = new Color(0, 0, 100);
  private static final Color DARK_GREEN = new Color(0, 100, 0);
  private static final Color DARK_RED = new Color(100, 0, 0);
  private static final Color BLUE = new Color(0, 0, 255);
  private static final Color GREEN = new Color(0, 255, 0);
  private static final Color RED = new Color(255, 0, 0);

  public ShimmerLED(Mote mote) {
    mspMote = (ShimmerMote) mote;

    IOUnit unit = mspMote.getCPU().getIOUnit("Port 4");
    if (unit instanceof IOPort) {
      ((IOPort) unit).addPortListener(new PortListener() {
        public void portWrite(IOPort source, int data) {
          System.out.println("Hallo!!!!");
          blueOn = (data & ShimmerNode.LED_YELLOW_MASK) == 0;
          greenOn = (data & ShimmerNode.LED_GREEN_MASK) == 0;
          redOn = (data & ShimmerNode.LED_RED_MASK) == 0;
          setChanged();
          notifyObservers();
        }
      });
    }
  }

  public boolean isAnyOn() {
    return blueOn || greenOn || redOn;
  }

  public boolean isGreenOn() {
    return greenOn;
  }

  public boolean isYellowOn()  {
    return blueOn; /* Returning blue */
  }

  public boolean isRedOn() {
    return redOn;
  }

  public JPanel getInterfaceVisualizer() {
    final JPanel panel = new JPanel() {
      @Override
      public void paintComponent(Graphics g) {
        super.paintComponent(g);

        int x = 20;
        int y = 25;
        int d = 25;

        if (isGreenOn()) {
          g.setColor(GREEN);
          g.fillOval(x, y, d, d);
          g.setColor(Color.BLACK);
          g.drawOval(x, y, d, d);
        } else {
          g.setColor(DARK_GREEN);
          g.fillOval(x + 5, y + 5, d-10, d-10);
        }

        x += 40;

        if (isRedOn()) {
          g.setColor(RED);
          g.fillOval(x, y, d, d);
          g.setColor(Color.BLACK);
          g.drawOval(x, y, d, d);
        } else {
          g.setColor(DARK_RED);
          g.fillOval(x + 5, y + 5, d-10, d-10);
        }

        x += 40;

        if (isYellowOn()) {
          g.setColor(BLUE);
          g.fillOval(x, y, d, d);
          g.setColor(Color.BLACK);
          g.drawOval(x, y, d, d);
        } else {
          g.setColor(DARK_BLUE);
          g.fillOval(x + 5, y + 5, d-10, d-10);
        }
      }
    };

    Observer observer;
    this.addObserver(observer = new Observer() {
      public void update(Observable obs, Object obj) {
        panel.repaint();
      }
    });

    // Saving observer reference for releaseInterfaceVisualizer
    panel.putClientProperty("intf_obs", observer);

    panel.setMinimumSize(new Dimension(140, 60));
    panel.setPreferredSize(new Dimension(140, 60));

    return panel;
  }

  public void releaseInterfaceVisualizer(JPanel panel) {
    Observer observer = (Observer) panel.getClientProperty("intf_obs");
    if (observer == null) {
      logger.fatal("Error when releasing panel, observer is null");
      return;
    }

    this.deleteObserver(observer);
  }

  public Collection<Element> getConfigXML() {
    return null;
  }

  public void setConfigXML(Collection<Element> configXML, boolean visAvailable) {
  }

}
