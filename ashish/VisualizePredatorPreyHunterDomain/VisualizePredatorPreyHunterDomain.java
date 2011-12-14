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

public class VisualizePredatorPreyHunterDomain {
	 public static void main(String[] args) throws IOException {
		  if ( args.length < 1 ) {
			  System.out.println( "Expecting path of file containing agent positions" );
		  }
		  String pathFileAgentPositions = args[0];
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
          frameVisualizeAgents.setVisible( true );
          GridWorld gridWorld = new GridWorld( 10, 10 );
          try {
        	  counter = 1;
	          for ( AgentPositions agentPositions : alAgentPositions ) {
	        	  System.out.println( "STEP " + counter );
	        	  agentCanvas.drawNext( gridWorld, agentPositions );
	        	  Thread.sleep( 300 );
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
	/*public void paint(Graphics g) {
		g.drawRect(10, 10, 10, 10);
		g.drawString("P", 10, 10);
		g.drawString("H", 200, 100);
		g.drawString("Q", 300, 100);		
	}*/
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
		// initialize for first drawing
		agentPositions.predator = new Position();
		agentPositions.prey = new Position();
		agentPositions.hunter = new Position();
		agentPositions.predator.x = 0;
		agentPositions.predator.y = 0;
		agentPositions.prey.x = 0;
		agentPositions.prey.y = 0;
		agentPositions.hunter.x = 0;
		agentPositions.hunter.y = 0;
		stepX = 10;
		stepY = 10;
		System.out.println( "Width: " + getWidth() );
		System.out.println( "Height: " + getHeight() );
		setMinimumSize( new Dimension( 500, 500) );
		setPreferredSize( new Dimension( 700, 700 ) );
		colorPredator = new Color( 255, 0, 0 );
		colorPrey = new Color( 0, 0, 0 );
		colorHunter = new Color( 0, 0, 255 );
	}
	public void paint( Graphics g ) {
		if ( null == agentPositions ) {
			System.out.println( "Trying to draw but no agentPositions" );
		} else {
			// you can change this to something else
			g.setColor( colorPredator );
			g.drawRect( agentPositions.predator.x, agentPositions.predator.y, stepX, stepY );
			g.setColor( colorPrey );
			g.drawOval( agentPositions.prey.x, agentPositions.prey.y, stepX, stepY );
			g.setColor( colorHunter );
			g.fillOval( agentPositions.hunter.x, agentPositions.hunter.y, stepX, stepY );
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
		this.agentPositions.predator.x = agentPositions.predator.x * stepX;
		this.agentPositions.predator.y = agentPositions.predator.y * stepY;
		this.agentPositions.prey.x = agentPositions.prey.x * stepX;
		this.agentPositions.prey.y = agentPositions.prey.y * stepY;
		this.agentPositions.hunter.x = agentPositions.hunter.x * stepX;
		this.agentPositions.hunter.y = agentPositions.hunter.y * stepY;
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

class Position {
	public int x;
	public int y;
}

class AgentPositions {
	public Position predator;
	public Position hunter;
	public Position prey;
	public String toString() {
		String separator = "\t";
		String terminate = "\n";
		String strPredator = "Predator( " + predator.x + ", " + predator.y + " )";
		String strPrey = "Prey( " + prey.x + ", " + prey.y + ")";
		String strHunter = "Hunter( " + hunter.x + ", " + hunter.y + " )";
		return strPredator + separator + strPrey + separator + strHunter;
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
			for ( Integer i : alInt ) {
				System.out.println( i );
			}
			if ( alInt.size() == 0 ) {
				break;
			}
			if ( alInt.size() != 6 ) {
				throw new IllegalArgumentException( "Number of elements in line of file " + pathFileAgentPositions + " is not equal to 6 but is " + alInt.size() );	
			}
			AgentPositions agentPositions = new AgentPositions();
			agentPositions.predator = new Position();
			agentPositions.prey = new Position();
			agentPositions.hunter = new Position();
			agentPositions.predator.x = alInt.get( 0 );
			agentPositions.predator.y = alInt.get( 1 );
			agentPositions.prey.x = alInt.get( 2 );
			agentPositions.prey.y = alInt.get( 3 );
			agentPositions.hunter.x = alInt.get( 4 );
			agentPositions.hunter.y = alInt.get( 5 );
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
