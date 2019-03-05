package Graph;

public class Port {	
	//connect port
	public static final int CON_PORT_SIZE = 10; 
	public static final int CON_PORT_SIZE_2 = CON_PORT_SIZE >> 1; 
	
	public int _dir;
	public BasicObject _obj;
	
	public Port(BasicObject object, int dir){
		_obj = object;
		_dir = dir;
	}
}
