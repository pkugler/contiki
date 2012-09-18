package se.sics.cooja.mspmote;

import java.io.File;
import org.apache.log4j.Logger;
import se.sics.cooja.Simulation;
import se.sics.mspsim.platform.shimmer.ShimmerNode;

public class ShimmerMote extends MspMote {
  private static Logger logger = Logger.getLogger(SkyMote.class);

  public ShimmerNode shimmerNode = null;

  public ShimmerMote(MspMoteType moteType, Simulation sim) {
    super(moteType, sim);
  }

  protected boolean initEmulator(File fileELF) {
    try {
      shimmerNode = new ShimmerNode();
      registry = shimmerNode.getRegistry();

      prepareMote(fileELF, shimmerNode);
    } catch (Exception e) {
      logger.fatal("Error when creating Shimmer mote: ", e);
      return false;
    }
    return true;
  }

  @Override
  public void idUpdated(int newID) {
   // shimmerNode.setNodeID(newID);

    /* Statically configured MAC addresses */
    /*configureWithMacAddressesTxt(newID);*/
  }

//  public SkyCoffeeFilesystem getFilesystem() {
//    return getInterfaces().getInterfaceOfType(SkyCoffeeFilesystem.class);
//  }

  @Override
  public String toString() {
    return "Shimmer " + getID();
  }

}
