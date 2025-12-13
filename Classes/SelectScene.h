#pragma once
#ifndef __SELECT_SCENE_H__
#define __SELECT_SCENE_H__

#include "cocos2d.h"

class SelectScene :public cocos2d::Scene {
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(SelectScene);
	//Ìø×ª
	void transitionToGameScene();
private:
	int _bgmAudioId;

    // ´´½¨UI
    void createStartButton();
	void createExitButton();
};

#endif// __SELECT_SCENE_H__