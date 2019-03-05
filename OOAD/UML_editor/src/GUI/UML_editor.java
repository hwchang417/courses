package GUI;
import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.Panel;
import java.util.ArrayList;

import javax.swing.BoxLayout;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;

import Button.AssociationBtn;
import Button.ClassBtn;
import Button.CompositionBtn;
import Button.GeneralizationBtn;
import Button.Mode;
import Button.SelectBtn;
import Button.UseCaseBtn;
import Graph.Shape;
import Menu.ChangeNameMenu;
import Menu.GroupMenu;
import Menu.UnGroupMenu;


public class UML_editor extends JFrame{
	
	private static final long serialVersionUID = 1L;

	//JFrame
	final int WINDOW_WIDTH = 640;
	final int WINDOW_HEIGHT = 500;
	final String TITLE_NAME = "UML editor";
	
	//button
	//ArrayList<Button> btn_list;

    //menu button
	//ArrayList<Menu> menu_btn_list;
    final String FILE_MENU_TITLE = "File";
    final String EDIT_MENU_TITLE = "Edit";
	
	public UML_editor(){

		Container container = getContentPane();

		//create canvas
		UML_Canvas canvas = new UML_Canvas();
		
		//create menu
		setJMenuBar(createMenuBar(canvas));

		//add panel to GUI 
		container.setLayout(new BorderLayout(2, 5));
		container.add(createButton(canvas), BorderLayout.WEST);
		container.add(canvas, BorderLayout.CENTER);

		//window general setting
		generalSetting();
	}
	
	JMenuBar createMenuBar(UML_Canvas canvas){
		
		JMenuBar menubar = new JMenuBar();
        JMenu file = new JMenu(FILE_MENU_TITLE);
        JMenu edit = new JMenu(EDIT_MENU_TITLE);
		
        //add menuItem to JMenu
        GroupMenu gm = new GroupMenu(canvas);
        UnGroupMenu ugm = new UnGroupMenu(canvas);
        ChangeNameMenu cnm = new ChangeNameMenu(canvas);
        
        gm.addActionListener(gm);
        ugm.addActionListener(ugm);
        cnm.addActionListener(cnm);
        
        edit.add(gm);
        edit.add(ugm);
        edit.add(cnm);
        
        menubar.add(file);
        menubar.add(edit);
        
        return menubar;
	}
	
	Panel createButton(UML_Canvas canvas){
		Panel pnl_button = new Panel();
		pnl_button.setLayout(new BoxLayout(pnl_button, BoxLayout.Y_AXIS));

		//add button to panel
		pnl_button.add(new SelectBtn(canvas));
		pnl_button.add(new AssociationBtn(canvas));
		pnl_button.add(new GeneralizationBtn(canvas));
		pnl_button.add(new CompositionBtn(canvas));
		pnl_button.add(new ClassBtn(canvas));
		pnl_button.add(new UseCaseBtn(canvas));
		return pnl_button;
	}
	
	private void generalSetting(){
		//window general setting
		setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
		setTitle(TITLE_NAME);
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		setVisible(true);
	}
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		UML_editor editor = new UML_editor();
	}
}
