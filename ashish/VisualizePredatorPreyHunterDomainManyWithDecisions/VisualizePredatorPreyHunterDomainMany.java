import java.awt.*;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import static java.awt.geom.AffineTransform.*;
import java.awt.geom.AffineTransform;
import javax.swing.JFrame;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.StringTokenizer;
import java.awt.image.BufferedImage;
import java.io.File;
import javax.imageio.ImageIO;
import java.awt.Color;

public class VisualizePredatorPreyHunterDomainMany {
	 public static void main(String[] args) throws IOException {
		  if ( args.length < 5 ) {
			  System.out.println( "Expecting path of file containing agent positions" );
        System.out.println( "Width of grid" );
        System.out.println( "Height of grid" );
        System.out.println( "Expecting delay between each draw" );
        System.out.println( "Expecting path of file containing agent decisions in the format x y decision" );
        System.out.println( "[Note] format for each line in file for agent positions: type x y type x y ..." );
		  }
		  String pathFileAgentPositions = args[0];
      int gridWidth = Integer.parseInt( args[1] );
      int gridHeight = Integer.parseInt( args[2] );
      int delay = Integer.parseInt( args[3] );
      String pathFileAgentDecisions = args[4];
      System.out.println( "File Positions: " + pathFileAgentPositions );
      System.out.println( "Grid Width: " + gridWidth );
      System.out.println( "Grid Height: " + gridHeight );
      System.out.println( "Delay: " + delay );
      System.out.println( "File Decisions: " + pathFileAgentDecisions );
      ExtractAgentPositions extractAgentPositions = new ExtractAgentPositions( pathFileAgentPositions );
      ArrayList<AgentPositions> alAgentPositions = extractAgentPositions.extract();
      System.out.println( "Showing agent positions" );
      int counter = 1;
      for (AgentPositions agentPositions : alAgentPositions) {
        System.out.println( counter + " " + agentPositions );
        counter++;
      }
      ExtractAgentDecisions extractAgentDecisions = new ExtractAgentDecisions( pathFileAgentDecisions );
      ArrayList<AgentDecisions> alAgentDecisions = extractAgentDecisions.extract();
      System.out.println( "Showing agent decisions" );
      counter = 1;
      for (AgentDecisions agentDecisions : alAgentDecisions) {
        System.out.println( counter + " " + agentDecisions );
        counter++;
      }
      FrameVisualizeAgents frameVisualizeAgents = new FrameVisualizeAgents( "Visualize Agents" );
      frameVisualizeAgents.setSize( 200, 200 );
      Container contentPane = frameVisualizeAgents.getContentPane();
      AgentCanvas agentCanvas = new AgentCanvas();
      contentPane.add( agentCanvas );
      agentCanvas.setMinimumSize( new Dimension( 100, 100 ) );
      agentCanvas.setPreferredSize( new Dimension( 100, 100 ) );
      agentCanvas.setBackground(Color.WHITE);
      frameVisualizeAgents.setDefaultCloseOperation( JFrame.EXIT_ON_CLOSE );
      frameVisualizeAgents.setVisible( true );
      GridWorld gridWorld = new GridWorld( gridWidth, gridHeight );
      try {
        counter = 0;
        for ( ; counter < alAgentDecisions.size(); counter++ ) {
          int counterNormalized = counter + 1;
          System.out.println( "STEP " + counterNormalized );
          AgentDecisions agentDecisions = alAgentDecisions.get( counter );
          AgentDecision agentDecision = agentDecisions.alAgentDecision.get( 0 );
          agentCanvas.drawNext( gridWorld, alAgentPositions.get( counter ), agentDecision );
          saveCanvasToFile(agentCanvas, counterNormalized);
          Thread.sleep( delay );
        }
        for ( ; counter < alAgentPositions.size(); counter++ ) {
          int counterNormalized = counter + 1;
          System.out.println( "STEP " + counterNormalized );
          agentCanvas.drawNext( gridWorld, alAgentPositions.get( counter ) );
          saveCanvasToFile(agentCanvas, counterNormalized);
          Thread.sleep( delay );
        }
      } catch ( InterruptedException ie ) {
        System.out.println( "ERROR: for loop interrupted" );
      }
      System.out.println( "FINISHED" );
	 }
	 
	 private static BufferedImage canvasToImage(Canvas cnvs) {
         int w = cnvs.getWidth();
         int h = cnvs.getHeight();
         int type = BufferedImage.TYPE_INT_RGB;
         BufferedImage image = new BufferedImage(w,h,type);
         Graphics2D g2 = image.createGraphics();
         g2.setPaint ( Color.WHITE/*new Color ( r, g, b )*/ );
         g2.fillRect ( 0, 0, image.getWidth(), image.getHeight() );
         cnvs.paint(g2);
         g2.dispose();
         return image;
     }
     
    private static void saveCanvasToFile(Canvas canvas, int stepNo) {
      try {
          // retrieve image
          BufferedImage bi = canvasToImage(canvas);
          File outputfile = new File("vid"+stepNo+".png");
          ImageIO.write(bi, "png", outputfile);
      } catch (IOException e) {

      }
    }
}

class FrameVisualizeAgents extends JFrame {
	public FrameVisualizeAgents(String title) {
		super(title);
		FrameVisualizeAgentsWindowAdapter adapter = new FrameVisualizeAgentsWindowAdapter(this);
		addWindowListener(adapter);
	}
}

class FrameVisualizeAgentsWindowAdapter extends WindowAdapter {
	FrameVisualizeAgents frameVisualizeAgents;
	public FrameVisualizeAgentsWindowAdapter( FrameVisualizeAgents frameVisualizeAgents ) {
		this.frameVisualizeAgents = frameVisualizeAgents;
	}
	public void windowClosing(WindowEvent we) {
		frameVisualizeAgents.setVisible( false );
	}
}

class AgentCanvas extends Canvas {
	AgentCanvas() {
		agentPositions = new AgentPositions();
		stepX = 10;
		stepY = 10;
		setMinimumSize( new Dimension( 500, 500) );
		setPreferredSize( new Dimension( 700, 700 ) );
		colorPredator = new Color( 255, 0, 0 );
		colorPrey = new Color( 0, 0, 0 );
		colorHunter = new Color( 0, 0, 255 );
    colorNetworkSelected = new Color( 255, 255, 0 );
    agentDecision = new AgentDecision();
		System.out.println( "Width: " + getWidth() );
		System.out.println( "Height: " + getHeight() );
    drawDecision = false;
	}

  void drawArrow(Graphics g1, int x1, int y1, int x2, int y2) {
    Graphics2D g = (Graphics2D) g1.create(); 
    double dx = x2 - x1, dy = y2 - y1;
    double angle = Math.atan2(dy, dx);
    int len = (int) Math.sqrt(dx*dx + dy*dy);
    AffineTransform at = AffineTransform.getTranslateInstance(x1, y1);
    at.concatenate(AffineTransform.getRotateInstance(angle));
    g.transform(at);

    // Draw horizontal arrow starting in (0, 0)
    int ARR_SIZE = 4;
    g.fillPolygon(new int[] {len, len-ARR_SIZE, len-ARR_SIZE, len},
                  new int[] {0, -ARR_SIZE, ARR_SIZE, 0}, 4);
  }

  private void drawArrow2(Graphics g, int x0, int y0, int x1,int y1){
	int deltaX = x1 - x0;
	int deltaY = y1 - y0;
	double frac = 0.2;

	g.drawLine(x0,y0,x1,y1);
	g.drawLine(x0 + (int)((1-frac)*deltaX + frac*deltaY),
		   y0 + (int)((1-frac)*deltaY - frac*deltaX),
		   x1, y1);
	g.drawLine(x0 + (int)((1-frac)*deltaX - frac*deltaY),
		   y0 + (int)((1-frac)*deltaY + frac*deltaX),
		   x1, y1);

    }


	public void paint( Graphics g ) {
		if ( null == agentPositions ) {
			System.out.println( "Trying to draw but no agentPositions" );
		} else {
			// you can change this to something else
      // drawing the network selected first 
      if ( drawDecision ) {
        g.setColor( colorNetworkSelected );
        int xNetworkSelected = agentDecision.x + ( stepX / 2 );
        int yNetworkSelected = agentDecision.y + ( stepY / 2 );
        int x1NetworkSelected = xNetworkSelected - stepX;
        int y1NetworkSelected = yNetworkSelected - stepY;
        int x2NetworkSelected = xNetworkSelected + stepX;
        int y2NetworkSelected = yNetworkSelected + stepY;
        g.fillOval( x1NetworkSelected, y1NetworkSelected, 2 * stepX, 2 * stepY );
      }
      // drawing the agents next
      for ( AgentPosition ap : agentPositions.alAgentPosition ) {
        switch( ap.type ) {
          case 0:  // PREY
            g.setColor( colorPrey );
            g.drawOval( ap.x, ap.y, stepX, stepY );
            break;
          case 1:  // PREDATOR
            g.setColor( colorPredator );
            g.drawRect( ap.x, ap.y, stepX, stepY );
            break;
          case 2:  // HUNTER
            g.setColor( colorHunter );
            g.fillOval( ap.x, ap.y, stepX, stepY );
            break;
          default: // UNKNOWN TYPE
            throw new IllegalArgumentException( "Unknown Agent Type encountered!" );
        }
      }
      if ( drawDecision ) {
        g.setColor( colorPredator );  
        for ( AgentPosition ap : agentPositions.alAgentPosition ) {
          if ( ap.type == 1 ) { // PREDATOR
            // NORTH
            int x1Arrow = ap.x + ( stepX / 2 );
            int y1Arrow = ap.y + ( stepY / 2 );
            int x2Arrow, y2Arrow;
            switch( agentDecision.action ) {
              case 0: // NORTH
                x2Arrow = x1Arrow;
                y2Arrow = y1Arrow + 30;
                break;
              case 1: // EAST 
                x2Arrow = x1Arrow + 30;
                y2Arrow = y1Arrow;
                break;
              case 2: // SOUTH 
                x2Arrow = x1Arrow;
                y2Arrow = y1Arrow - 30;
                break;
              case 3: // WEST 
                x2Arrow = x1Arrow - 30;
                y2Arrow = y1Arrow;
                break;
              case 4: // STAY
                x2Arrow = x1Arrow;
                y2Arrow = y1Arrow;
                break;
              default: // UNKNOWN ACTION
                throw new IllegalArgumentException( "Unknown Agent Action encountered!" );
            }
            drawArrow2( g, x1Arrow, y1Arrow, x2Arrow, y2Arrow ); 
          }
        }
      }
		}
	}
	public void drawNext( GridWorld gridWorld, AgentPositions agentPositions ) {
    drawDecision = false;
		this.agentPositions = agentPositions;
		System.out.println( "Width: " + getWidth() + ", Height: " + getHeight() );
		System.out.println( agentPositions );
		int stepX, stepY;
		stepX = getWidth() / gridWorld.width;
		stepY = getHeight() / gridWorld.height;
		if ( stepX <= 0 || stepY <= 0 ) {
			throw new IllegalArgumentException( "GridWorld passed is much larger than canvas size!" );
		}
    for ( AgentPosition ap : this.agentPositions.alAgentPosition ) {
      ap.x = ap.x * stepX;
      ap.y = ap.y * stepY;
    }
		repaint();
	}
	public void drawNext( GridWorld gridWorld, AgentPositions agentPositions, AgentDecision agentDecision ) {
    drawDecision = true;
		this.agentPositions = agentPositions;
    this.agentDecision = agentDecision;
		System.out.println( "Width: " + getWidth() + ", Height: " + getHeight() );
		System.out.println( agentPositions );
		int stepX, stepY;
		stepX = getWidth() / gridWorld.width;
		stepY = getHeight() / gridWorld.height;
		if ( stepX <= 0 || stepY <= 0 ) {
			throw new IllegalArgumentException( "GridWorld passed is much larger than canvas size!" );
		}
    for ( AgentPosition ap : this.agentPositions.alAgentPosition ) {
      ap.x = ap.x * stepX;
      ap.y = ap.y * stepY;
    }
    this.agentDecision.x = this.agentDecision.x * stepX;
    this.agentDecision.y = this.agentDecision.y * stepY;
		repaint();
	}
	AgentPositions agentPositions;
	int stepX;
	int stepY;
	Color colorPredator;
	Color colorPrey;
	Color colorHunter;
  Color colorNetworkSelected;
  AgentDecision agentDecision;
  boolean drawDecision;
}

class GridWorld {
	public int width;
	public int height;
	GridWorld( int width, int height ) {
		this.width = width;
		this.height = height;
	}
}

class AgentPosition {
  public int type;
	public int x;
	public int y;
}

class AgentPositions {
	public ArrayList<AgentPosition> alAgentPosition = new ArrayList<AgentPosition>();
	public String toString() {
		String separator = "\t";
		String terminate = "\n";
    ArrayList<String> alStrAgentPosition = new ArrayList<String>();
    for ( AgentPosition ap : alAgentPosition ) {
      String strPosition = new String( ap.x + ", " + ap.y );
      String strType; 
      switch( ap.type ) { // PREY
        case 0:
          strType = new String( "Prey" );
          break;
        case 1:
          strType = new String( "Predator" );
          break;
        case 2:
          strType = new String( "Hunter" );
          break;
        default:
          throw new IllegalArgumentException( "Unknown Agent Type encountered!" );
      }
      alStrAgentPosition.add( new String( strType + "( " + strPosition + " )" ) );
    }
    String sFinal = "";
    for ( String strAgentPosition : alStrAgentPosition ) {
      sFinal = sFinal + separator + strAgentPosition;
    }
		return sFinal; 
	}
}

class ExtractAgentPositions {
	String pathFileAgentPositions;
	public ExtractAgentPositions( String pathFileAgentPositions ) {
		this.pathFileAgentPositions = pathFileAgentPositions;
	}
	public ArrayList<AgentPositions> extract( ) throws FileNotFoundException, IOException {
		ArrayList<AgentPositions> alAgentPositions = new ArrayList<AgentPositions>();
		FileReader fr = new FileReader( pathFileAgentPositions );
		BufferedReader br = new BufferedReader( fr );
		String strLine;
		while ( null != ( strLine = br.readLine() ) ) {
			ArrayList<Integer> alInt = extractNumbers( strLine );
			// TEST
			//for ( Integer i : alInt ) {
				//System.out.println( i );
			//}
			if ( alInt.size() == 0 ) {
				break;
			}
			if ( alInt.size() % 3 != 0 ) {
				throw new IllegalArgumentException( "Number of elements in line of file " + pathFileAgentPositions + " is not a multiple of 3. The size is " + alInt.size() );	
			}
      AgentPositions agentPositions = new AgentPositions();
      for ( int i = 0; i < alInt.size(); i+=3 ) {
        AgentPosition agentPosition = new AgentPosition();
        agentPosition.type = alInt.get( i );
        agentPosition.x = alInt.get( i + 1 );
        agentPosition.y = alInt.get( i + 2 );
        agentPositions.alAgentPosition.add( agentPosition ); 
      }
			alAgentPositions.add( agentPositions );
		}
		return alAgentPositions;
	}
	public static ArrayList<Integer> extractNumbers( String str ) {
		ArrayList<Integer> alInt = new ArrayList<Integer>();
		StringTokenizer st = new StringTokenizer( str );
		while ( st.hasMoreTokens() ) {
			String strNumber = st.nextToken();
			int number = Integer.parseInt( strNumber );
			alInt.add( number );
		}
		return alInt;
	}
}

class AgentDecision {
  public int x;
  public int y;
  public int action;
}

class AgentDecisions {
	public ArrayList<AgentDecision> alAgentDecision = new ArrayList<AgentDecision>();
	public String toString() {
		String separator = "\t";
		String terminate = "\n";
    ArrayList<String> alStrAgentDecision = new ArrayList<String>();
    for ( AgentDecision ad : alAgentDecision ) {
      String strAgent = new String( ad.x + ", " + ad.y );
      String strAction; 
      switch( ad.action ) { // ACTION 
        case 0:
          strAction = new String( "North" );
          break;
        case 1:
          strAction = new String( "East" );
          break;
        case 2:
          strAction = new String( "South" );
          break;
        case 3:
          strAction = new String( "West" );
          break;
        case 4:
          strAction = new String( "Stay" );
          break;
        default:
          throw new IllegalArgumentException( "Unknown Agent Type <" + ad.action + "> encountered!" );
      }
      alStrAgentDecision.add( new String( strAction + "( " + strAgent + " )" ) );
    }
    String sFinal = "";
    for ( String strAgentDecision : alStrAgentDecision ) {
      sFinal = sFinal + separator + strAgentDecision;
    }
		return sFinal; 
	}
}

class ExtractAgentDecisions {
	String pathFileAgentDecisions;
	public ExtractAgentDecisions( String pathFileAgentDecisions ) {
		this.pathFileAgentDecisions = pathFileAgentDecisions;
	}
	public ArrayList<AgentDecisions> extract( ) throws FileNotFoundException, IOException {
		ArrayList<AgentDecisions> alAgentDecisions = new ArrayList<AgentDecisions>();
		FileReader fr = new FileReader( pathFileAgentDecisions );
		BufferedReader br = new BufferedReader( fr );
		String strLine;
		while ( null != ( strLine = br.readLine() ) ) {
			ArrayList<Integer> alInt = extractNumbers( strLine );
			// TEST
			//for ( Integer i : alInt ) {
				//System.out.println( i );
			//}
			if ( alInt.size() == 0 ) {
				break;
			}
			if ( alInt.size() % 3 != 0 ) {
				throw new IllegalArgumentException( "Number of elements in line of file " + pathFileAgentDecisions + " is not a multiple of 3. The size is " + alInt.size() );	
			}
      AgentDecisions agentDecisions = new AgentDecisions();
      for ( int i = 0; i < alInt.size(); i+=3 ) {
        AgentDecision agentDecision = new AgentDecision();
        agentDecision.x = alInt.get( i + 0 );
        agentDecision.y = alInt.get( i + 1 );
        agentDecision.action = alInt.get( i + 2 );
        agentDecisions.alAgentDecision.add( agentDecision ); 
      }
			alAgentDecisions.add( agentDecisions );
		}
		return alAgentDecisions;
	}
	public static ArrayList<Integer> extractNumbers( String str ) {
		ArrayList<Integer> alInt = new ArrayList<Integer>();
		StringTokenizer st = new StringTokenizer( str );
		while ( st.hasMoreTokens() ) {
			String strNumber = st.nextToken();
			int number = Integer.parseInt( strNumber );
			alInt.add( number );
		}
		return alInt;
	}
}

