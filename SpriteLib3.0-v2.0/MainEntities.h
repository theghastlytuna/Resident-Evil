#ifndef __MAINENTITIES_H__
#define __MAINENTITIES_H__

class MainEntities
{
public:
	//Getters
	static int MainCamera();
	static int MainPlayer();
	static int MainUI();
	static int MainAmmoUI();

	//Setters
	static void MainCamera(int main);
	static void MainPlayer(int main);
	static void MainUI(int main);
	static void MainAmmoUI(int main);

private:
	static int m_mainCamera;
	static int m_mainPlayer;
	static int m_mainUI;
	static int m_mainAmmoUI;
};

#endif // !__MAINENTITIES_H__
