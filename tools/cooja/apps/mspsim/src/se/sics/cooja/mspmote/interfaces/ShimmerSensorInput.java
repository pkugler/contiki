package se.sics.cooja.mspmote.interfaces;

import org.jdom.Element;
import se.sics.cooja.ClassDescription;
import se.sics.cooja.Mote;
import se.sics.cooja.MoteInterface;

import se.sics.cooja.mspmote.ShimmerMote;
import se.sics.mspsim.chip.SensorInput;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.util.Collection;
import java.util.Observable;
import java.util.Observer;

@ClassDescription("Shimmer sensor input")
public class ShimmerSensorInput extends MoteInterface {

  private SensorInput input;

  public ShimmerSensorInput(Mote mote) {
    ShimmerMote shimmer = (ShimmerMote) mote;
    input = shimmer.shimmerNode.getSensorInput();
  }

  @Override
  public JPanel getInterfaceVisualizer() {
    final JPanel panel = new JPanel();
    panel.setLayout(new BoxLayout(panel, BoxLayout.PAGE_AXIS));

    final JLabel label = new JLabel("No file selected");
    panel.add(label);

    final JFileChooser fc = new JFileChooser();
    JButton button = new JButton();
    button.setText("Open file");
    button.addActionListener(new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent actionEvent) {
        int returnVal = fc.showOpenDialog(panel);

        if (returnVal == JFileChooser.APPROVE_OPTION) {
          label.setText(fc.getSelectedFile().getAbsolutePath());
          try {
            input.openFile(fc.getSelectedFile().getAbsolutePath());
          } catch (IOException e) {
            e.printStackTrace();
          }
        }
      }
    });
    panel.add(button);

    final JCheckBox checkAccelerometer = new JCheckBox("Use accelerometer",
        input.isAccelerometerEnabled());
    checkAccelerometer.addActionListener(new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent actionEvent) {
        input.setAccelerometerEnabled(checkAccelerometer.isSelected());
      }
    });

    final JCheckBox checkGyroscope = new JCheckBox("Use gyroscope",
        input.isGyroscopeEnabled());
    checkGyroscope.addActionListener(new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent actionEvent) {
        input.setGyroscopeEnabled(checkGyroscope.isSelected());
      }
    });

    panel.add(checkAccelerometer);
    panel.add(checkGyroscope);

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
  }

  @Override
  public Collection<Element> getConfigXML() {
    return null;
  }

  @Override
  public void setConfigXML(Collection<Element> configXML, boolean visAvailable) {
  }
}
