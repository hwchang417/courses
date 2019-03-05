package Menu;

import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.util.ArrayList;

import Button.Mode;
import GUI.UML_Canvas;
import Graph.Composite;
import Graph.Shape;

public class GroupMenu extends Menu{
	
	private static final long serialVersionUID = 1L;

	public GroupMenu(UML_Canvas canvas){
		super("Group", canvas);
        this.setMnemonic(KeyEvent.VK_G);
	}
    
	public void actionPerformed(ActionEvent arg0) {
		// TODO Auto-generated method stub

		int min_d = Integer.MAX_VALUE;
		int index = 0;
		ArrayList<Shape> list = _canvas.getList();
		ArrayList<Integer> remove_index_list = new ArrayList<Integer>();
		Composite composite = new Composite(Mode.MAX_DEPTH - 1);
		
		for(Shape element : list){
			if(element.isSelected()){
				composite.add(element, element.getWidth(), element.getHeight());
				if(min_d > element.getDepth()){
				  min_d = element.getDepth();
				}
				remove_index_list.add(index);
			}
			index++;
		}
		
		if(composite.getCount() > 1){
			for(int i = remove_index_list.size() - 1;i >= 0;i--){
				list.remove(remove_index_list.get(i).intValue());
			}
			composite.setDepth(min_d - 1);
			list.add(composite);
			
			_canvas.repaint();
		}
	}

}
