package se.sics.cooja.mspmote;

import java.awt.Container;
import java.awt.Image;
import java.awt.MediaTracker;
import java.awt.Toolkit;
import java.io.File;
import java.net.URL;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JOptionPane;
import org.apache.log4j.Logger;
import se.sics.cooja.AbstractionLevelDescription;
import se.sics.cooja.ClassDescription;
import se.sics.cooja.GUI;
import se.sics.cooja.MoteInterface;
import se.sics.cooja.MoteType;
import se.sics.cooja.Simulation;
import se.sics.cooja.dialogs.CompileContiki;
import se.sics.cooja.dialogs.MessageList;
import se.sics.cooja.dialogs.MessageList.MessageContainer;
import se.sics.cooja.interfaces.MoteAttributes;
import se.sics.cooja.interfaces.Position;
import se.sics.cooja.mspmote.interfaces.*;

@ClassDescription("Shimmer mote")
@AbstractionLevelDescription("Emulated level")
public class ShimmerMoteType extends MspMoteType {
  private static Logger logger = Logger.getLogger(ShimmerMoteType.class);

  protected MspMote createMote(Simulation simulation) {
    return new ShimmerMote(this, simulation);
  }

  public boolean configureAndInit(Container parentContainer, Simulation simulation, boolean visAvailable)
  throws MoteType.MoteTypeCreationException {

    /* SPECIAL CASE: Cooja started in applet.
     * Use preconfigured Contiki firmware */
    if (GUI.isVisualizedInApplet()) {
      String firmware = GUI.getExternalToolsSetting("SHIMMER_FIRMWARE", "");
      if (!firmware.equals("")) {
        setContikiFirmwareFile(new File(firmware));
        JOptionPane.showMessageDialog(GUI.getTopParentContainer(),
            "Creating mote type from precompiled firmware: " + firmware,
            "Compiled firmware file available", JOptionPane.INFORMATION_MESSAGE);
      } else {
        JOptionPane.showMessageDialog(GUI.getTopParentContainer(),
            "No precompiled firmware found",
            "Compiled firmware file not available", JOptionPane.ERROR_MESSAGE);
        return false;
      }
    }

    /* If visualized, show compile dialog and let user configure */
    if (visAvailable) {

      /* Create unique identifier */
      if (getIdentifier() == null) {
        int counter = 0;
        boolean identifierOK = false;
        while (!identifierOK) {
          identifierOK = true;

          counter++;
          setIdentifier("shimmer" + counter);

          for (MoteType existingMoteType : simulation.getMoteTypes()) {
            if (existingMoteType == this) {
              continue;
            }
            if (existingMoteType.getIdentifier().equals(getIdentifier())) {
              identifierOK = false;
              break;
            }
          }
        }
      }

      /* Create initial description */
      if (getDescription() == null) {
        setDescription("Shimmer Mote Type #" + getIdentifier());
      }

      return MspCompileDialog.showDialog(parentContainer, simulation, this, "shimmer");
    }

    /* Not visualized: Compile Contiki immediately */
    if (getIdentifier() == null) {
      throw new MoteType.MoteTypeCreationException("No identifier");
    }

    final MessageList compilationOutput = new MessageList();

    if (getCompileCommands() != null) {
      /* Handle multiple compilation commands one by one */
      String[] arr = getCompileCommands().split("\n");
      for (String cmd: arr) {
        if (cmd.trim().isEmpty()) {
          continue;
        }

        try {
          CompileContiki.compile(
              cmd,
              null,
              null /* Do not observe output firmware file */,
              getContikiSourceFile().getParentFile(),
              null,
              null,
              compilationOutput,
              true
          );
        } catch (Exception e) {
          MoteType.MoteTypeCreationException newException =
            new MoteType.MoteTypeCreationException("Mote type creation failed: " + e.getMessage());
          newException = (MoteType.MoteTypeCreationException) newException.initCause(e);
          newException.setCompilationOutput(compilationOutput);

          /* Print last 10 compilation errors to console */
          MessageContainer[] messages = compilationOutput.getMessages();
          for (int i=messages.length-10; i < messages.length; i++) {
            if (i < 0) {
              continue;
            }
            logger.fatal(">> " + messages[i]);
          }

          logger.fatal("Compilation error: " + e.getMessage());
          throw newException;
        }
      }
    }

    if (getContikiFirmwareFile() == null ||
        !getContikiFirmwareFile().exists()) {
      throw new MoteType.MoteTypeCreationException("Contiki firmware file does not exist: " + getContikiFirmwareFile());
    }
    return true;
  }

  public Icon getMoteTypeIcon() {
    Toolkit toolkit = Toolkit.getDefaultToolkit();
    URL imageURL = this.getClass().getClassLoader().getResource("images/sky.jpg");
    Image image = toolkit.getImage(imageURL);
    MediaTracker tracker = new MediaTracker(GUI.getTopParentContainer());
    tracker.addImage(image, 1);
    try {
      tracker.waitForAll();
    } catch (InterruptedException ex) {
    }
    if (image.getHeight(GUI.getTopParentContainer()) > 0 && image.getWidth(GUI.getTopParentContainer()) > 0) {
      image = image.getScaledInstance((200*image.getWidth(GUI.getTopParentContainer())/image.getHeight(GUI.getTopParentContainer())), 200, Image.SCALE_DEFAULT);
      return new ImageIcon(image);
    }

    return null;
  }

  public Class<? extends MoteInterface>[] getAllMoteInterfaceClasses() {
    return new Class[] {
        Position.class,
        MoteAttributes.class,
        MspClock.class,
        MspMoteID.class,
        MspLED.class,
        MspSerial.class,
        ShimmerAccelerometer.class,
        ShimmerSensorInput.class,
        MspDebugOutput.class /* EXPERIMENTAL: Enable me for COOJA_DEBUG(..) */
    };
  }

  public File getExpectedFirmwareFile(File source) {
    File parentDir = source.getParentFile();
    String sourceNoExtension = source.getName().substring(0, source.getName().length()-2);

    return new File(parentDir, sourceNoExtension + ".shimmer");
  }

  protected String getTargetName() {
    return "shimmer";
  }

}
