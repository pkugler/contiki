package se.sics.cooja.mspmote.interfaces;

import org.jdom.Element;
import se.sics.cooja.ClassDescription;
import se.sics.cooja.Mote;
import se.sics.cooja.MoteInterface;
import se.sics.cooja.mspmote.ShimmerMote;

import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Collection;
import java.util.Observable;
import java.util.Observer;

@ClassDescription("Accelerometer")
public class ShimmerAccelerometer extends MoteInterface {

  private ShimmerMote shimmerMote;

  public ShimmerAccelerometer(Mote mote) {
    shimmerMote = (ShimmerMote) mote;
  }

  @Override
  public JPanel getInterfaceVisualizer() {
    final JPanel panel = new JPanel();
    panel.setLayout(new BoxLayout(panel, BoxLayout.PAGE_AXIS));

    final String[] axisName = { "X: ", "Y: ", "Z : "};
    for (int i = 0; i < 3; i++) {
      final int axisIndex = i;

      final JLabel adcLabel = new JLabel(axisName[axisIndex], JLabel.LEFT);
      final JSlider adc = new JSlider(JSlider.HORIZONTAL, 0, 4095, 2048);
      final JLabel adcValue = new JLabel("2048", JLabel.RIGHT);

      final ChangeListener adcListener = new ChangeListener() {
        @Override
        public void stateChanged(ChangeEvent changeEvent) {
          final int value = adc.getValue();
          final double accValue = (value - 2047.0) / 2047.0;
          adcValue.setText(Integer.toString(value));
          switch (axisIndex) {
            case 0:
              shimmerMote.shimmerNode.getAccelerometer().setX(accValue);
              break;
            case 1:
              shimmerMote.shimmerNode.getAccelerometer().setY(accValue);
              break;
            case 2:
              shimmerMote.shimmerNode.getAccelerometer().setZ(accValue);
              break;
          }
        }
      };

      JPanel axis = new JPanel();
      axis.setLayout(new BoxLayout(axis, BoxLayout.X_AXIS));
      axis.add(adcLabel);
      axis.add(adc);
      axis.add(adcValue);
      adc.addChangeListener(adcListener);

      panel.add(axis);
    }

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

  @Override
  public void releaseInterfaceVisualizer(JPanel panel) {
    //To change body of implemented methods use File | Settings | File Templates.
  }

  @Override
  public Collection<Element> getConfigXML() {
    return null;  //To change body of implemented methods use File | Settings | File Templates.
  }

  @Override
  public void setConfigXML(Collection<Element> configXML, boolean visAvailable) {
    //To change body of implemented methods use File | Settings | File Templates.
  }
}
