package sample;
import robocode.*;
import robocode.util.Utils;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;


public class GeneticRobot extends AdvancedRobot
{
	static boolean first_time = false;
	static int count = 0; 

    private double myShotAngle; 
    private double myShotEnergy;
    private double myMoveAngle;
    private double myMoveDistance; 
 
    private double x, y, dr, tr, w, dh, a, GH, h, d, e, E;
	/*
	+ x; y – координаты танка соперника относительно нашего танка;
	+ dr – расстояние, которое осталось “доехать” нашему танку
	+ tr – угол, на который осталось повернуться нашему танку;
	+ w – расстояние от нашего танка до края поля;
	+ dh – угол между направлением на танк соперника и пушкой нашего танка;
	+ GH – угол поворота пушки нашего танка;
	+ h – направление движения танка соперника;
	+ d – расстояние между нашим танком и танком соперника;
	+ e – энергия танка соперника;
	+ E – энергия нашего танка;
	*/    
        
        
 	int i;
	static int len = 41; // длина генома
 	
	Environment env;
	
	//хромосомы
	static int[] f_1= new int[len];
	static int[] f_2 = new int[len];
	static int[] f_3 = new int[len];
	static int[] f_4 = new int[len];
	
	
	//соответсвующие хромосомам деревья
	static int[][] tree_f1 = new int[len][4];	
	static int[][] tree_f2 = new int[len][4];	
	static int[][] tree_f3 = new int[len][4];	
	static int[][] tree_f4 = new int[len][4];
	
    public void run() {
		//если первый раунд боя - считываем хромосомы
		if (!first_time) {

			first_time = true;		
		    try {

		        BufferedReader r = new BufferedReader(new FileReader(getDataFile("chromosome.dat")));

				String[] f1 = r.readLine().split(" ");
				String[] f2 = r.readLine().split(" ");
				String[] f3 = r.readLine().split(" ");
				String[] f4 = r.readLine().split(" ");
			
				for(i = 0; i < len; i++) {
					f_1[i] = Integer.parseInt(f1[i]);
					f_2[i] = Integer.parseInt(f2[i]);
					f_3[i] = Integer.parseInt(f3[i]);
					f_4[i] = Integer.parseInt(f4[i]);
				}

		    } catch (IOException exc) {
				f_1 = new int[0];
				f_2 = new int[0];
				f_3 = new int[0];
				f_4 = new int[0];
				
		    } catch (NumberFormatException exc) {
				f_1 = new int[0];
				f_2 = new int[0];
				f_3 = new int[0];
				f_4 = new int[0];
			}			
			
			//строим связи (из массива делаем подобие дерева выражения)
			buildTreeFromChromosome(tree_f1,f_1);
			buildTreeFromChromosome(tree_f2,f_2);
			buildTreeFromChromosome(tree_f3,f_3);
			buildTreeFromChromosome(tree_f4,f_4);
			
		}  
		while(true) {
			
			setTurnRadarRightRadians(2*Math.PI);
			//определяем состояние
           	dr = getDistanceRemaining();
            tr = getTurnRemainingRadians();
			w = minWall();
			GH = getGunHeadingRadians();
            E = getEnergy();
          
          	//состояние боя
			env = new Environment(x, y, dr, tr, w, dh, a, GH, h, d, e, E);

			//по хромосам и состоянию считает действия
			myShotAngle = getMyValueFromChromosome(tree_f1, f_1);
			myShotEnergy = getMyValueFromChromosome(tree_f2, f_2);
			myMoveAngle = getMyValueFromChromosome(tree_f3, f_3);
			myMoveDistance = getMyValueFromChromosome(tree_f4, f_4);
			
			setTurnGunRightRadians(myShotAngle);			
			
			if (getGunHeat() == 0 && myShotEnergy > 0)
				setFire(myShotEnergy);

			setAhead(myMoveDistance);

			setTurnRight(myMoveAngle);
			
			execute();
        }
    }


    public void onScannedRobot(ScannedRobotEvent event) {
		a = Utils.normalRelativeAngle(getHeadingRadians() + event.getBearingRadians());
		e = event.getEnergy();
        d = event.getDistance();
        x = d * Math.sin(a);
        y = d * Math.cos(a);
		h = event.getHeadingRadians();
       	dh= -Utils.normalRelativeAngle(getGunHeadingRadians()-a);
    }

    public void onHitByWall(HitByBulletEvent e) {
        turnLeft(180);
    }

	public void buildTreeFromChromosome(int tree[][], int[] f) {
   		int k, r;
		int link = 1;
    

		
		for(k = 0; k < len; k++){
			r = 0;
			switch(f[k])
			{
				
				case 0:
				case 1:
					tree[k][r++] = link++;
				break;
					
				case 2:
				case 4:
				case 6:
					tree[k][r++] = link++;
					tree[k][r++] = link++;
				break;
					
				case 3:
				case 5:
					tree[k][r++] = link++;
					tree[k][r++] = link++;
					tree[k][r++] = link++;
				break;
					
				case 7:
					tree[k][r++] = link++;
					tree[k][r++] = link++;
					tree[k][r++] = link++;
					tree[k][r++] = link++;
				break;
				
				default:
				break;
					
			}
		}
	
	}

    public double getMyValueFromChromosome(int tree[][], int[] f) {
		double result;
		result = getTreeValue(f, tree, 0);
        return result;
    }


	public double getTreeValue(int[] f, int[][] links, int node) {
		double r1, r2, r3, r4;
		double res;
		
		r1 = 0;
		r2 = 0;
		r3 = 0;
		r4 = 0;
				
		switch(f[node]){
			case 7:
				r4 = getTreeValue(f,links,links[node][3]);

			case 3:
			case 5:
				r3 = getTreeValue(f,links,links[node][2]);

			case 2:
			case 4:
			case 6:
				r2 = getTreeValue(f,links,links[node][1]);
			
			case 0:
			case 1:
				r1 = getTreeValue(f,links,links[node][0]);
			break;	
			
			default:
				r1 = 0;
				r2 = 0;
				r3 = 0;
				r4 = 0;
			break;
		}
	
		
		switch(f[node]){
			case 0:
				res = 1/(1+Math.exp(-r1));		
			break;

			case 1:
				res = -r1;	
			break;

			case 2:
				res = r1+r2;	
			break;

			case 3:
				res = r1+r2+r3;	
			break;

			case 4:
				res = r1*r2;
			break;

			case 5:
				res = r1*r2*r3;	
			break;

			case 6:
				res = (r1 > r2) ? r2 : r1;
			break;

			case 7:
				res = (r1 > r2) ? r3 : r4;
			break;

			case 8:
				res = env.x;
			break;

			case 9:
				res = env.y;
			break;

			case 10:
				res = env.dr;
			break;

			case 11:
				res = env.tr;
			break;

			case 12:
				res = env.w;
			break;

			case 13:
				res = env.dh;
			break;

			case 14:
				res = env.GH;
			break;

			case 15:
				res = env.h;
			break;

			case 16:
				res = env.d;
			break;

			case 17:
				res = env.e;
			break;

			case 18:
				res = env.E;
			break;

			case 19:
				res = 0;
			break;

			case 20:
				res = 0.5;
			break;

			case 21:
				res = 1;
			break;

			case 22:
				res = 2;
			break;

			case 23:
				res = 5;
			break;

			case 24:
				res = 10;
			break;
			
			default:
				res = 0;
			break;
		}	
							
		return res;
	}


    private double minWall() {
        return Math.min(
            Math.min(
                        getX(),
                           getBattleFieldWidth() - getX()
            ),
            Math.min(
                        getY(),
                           getBattleFieldHeight() - getY()
            )
        );
    }
 
}


class Environment{
    double x, y, dr, tr, w, dh, a, GH, h, d, e, E;
	Environment(double x, double y, double dr, double tr, double w, double dh, double a, double GH, double h, double d, double e, double E) {
		this.x=x;
		this.y=y;
		this.dr=dr;
		this.tr=tr;
		this.w=w;
		this.dh=dh;
		this.a=a;
		this.GH=GH;
		this.h=h;
		this.d=d;
		this.e=e;
		this.E=E;
	}

}																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																