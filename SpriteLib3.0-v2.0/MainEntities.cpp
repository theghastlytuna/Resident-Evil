#include "MainEntities.h"

int MainEntities::m_mainCamera = 0;
int MainEntities::m_mainPlayer = 0;
int MainEntities::m_mainUI = 0;
int MainEntities::m_mainAmmoUI = 0;

int MainEntities::MainCamera()
{
	return m_mainCamera;
}

int MainEntities::MainPlayer()
{
	return m_mainPlayer;
}

int MainEntities::MainUI()
{
	return m_mainUI;
}

int MainEntities::MainAmmoUI()
{
	return m_mainAmmoUI;
}

void MainEntities::MainCamera(int main)
{
	m_mainCamera = main;
}

void MainEntities::MainPlayer(int main)
{
	m_mainPlayer = main;
}

void MainEntities::MainUI(int main)
{
	m_mainUI = main;
}

void MainEntities::MainAmmoUI(int main)
{
	m_mainAmmoUI = main;
}