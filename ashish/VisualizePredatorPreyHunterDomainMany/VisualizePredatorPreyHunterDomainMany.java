import java.awt.*;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import javax.swing.JFrame;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.StringTokenizer;

public class VisualizePredatorPreyHunterDomainMany {
	 public static void main(String[] args) throws IOException {
		  if ( args.length < 4 ) {
			  System.out.println( "Expecting path of file containing agent positions" );
        System.out.println( "Width of grid" );
        System.out.println( "Height of grid" );
        System.out.println( "Expecting delay between each draw" );
        System.out.println( "[Note] format for each line in file: type x y type x y ..." );
		  }
		  String pathFileAgentPositions = args[0];
      int gridWidth = Integer.parseInt( args[1] );
      int gridHeight = Integer.parseInt( args[2] );
      int delay = Integer.parseInt( args[3] );
      System.out.println( "File: " + pathFileAgentPositions );
      System.out.println( "Grid Width: " + gridWidth );
      System.out.println( "Grid Height: " + gridHeight );
      System.out.println( "Delay: " + delay );
      ExtractAgentPositions extractAgentPositions = new ExtractAgentPositions( pathFileAgentPositions );
      ArrayList<AgentPositions> alAgentPositions = extractAgentPositions.extract();
      System.out.println( "Showing agent positions" );
      int counter = 1;
      for (AgentPositions agentPositions : alAgentPositions) {
        System.out.println( counter + " " + agentPositions );
        counter++;
      }
      FrameVisualizeAgents frameVisualizeAgents = new FrameVisualizeAgents( "Visualize Agents" );
      frameVisualizeAgents.setSize( 200, 200 );
      Container contentPane = frameVisualizeAgents.getContentPane();
      AgentCanvas agentCanvas = new AgentCanvas();
      contentPane.add( agentCanvas );
      agentCanvas.setMinimumSize( new Dimension( 100, 100 ) );
      agentCanvas.setPreferredSize( new Dimension( 100, 100 ) );
      frameVisualizeAgents.setDefaultCloseOperation( JFrame.EXIT_ON_CLOSE );
      frameVisualizeAgents.setVisible( true );
      GridWorld gridWorld = new GridWorld( gridWidth, gridHeight );
      try {
        counter = 1;
        for ( AgentPositions agentPositions : alAgentPositions ) {
          System.out.println( "STEP " + counter );
          agentCanvas.drawNext( gridWorld, agentPositions );
          Thread.sleep( delay );
          counter++;
        }
      } catch ( InterruptedException ie ) {
        System.out.println( "ERROR: for loop interrupted" );
      }
      System.out.println( "FINISHED" );
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
		System.out.println( "Width: " + getWidth() );
		System.out.println( "Height: " + getHeight() );
	}
	public void paint( Graphics g ) {
		if ( null == agentPositions ) {
			System.out.println( "Trying to draw but no agentPositions" );
		} else {
			// you can change this to something else
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
		}
	}
	public void drawNext( GridWorld gridWorld, AgentPositions agentPositions ) {
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
	AgentPositions agentPositions;
	int stepX;
	int stepY;
	Color colorPredator;
	Color colorPrey;
	Color colorHunter;
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
