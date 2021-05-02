#include "GameScene.h"

USING_NS_CC;

GameScene::GameScene() {
    _c3sButton = nullptr;
    _midObj = nullptr;
    _runner = nullptr;
    _enemy = nullptr;
}

GameScene::~GameScene() {
    CC_SAFE_DELETE(_c3sButton);
    CC_SAFE_DELETE(_midObj);
    CC_SAFE_DELETE(_runner);
    CC_SAFE_DELETE(_enemy);
    this->removeAllChildren();
    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("scene101.plist");
    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("scene101bg.plist");
    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile("gamescene.plist");
    Director::getInstance()->getTextureCache()->removeUnusedTextures();
}

Scene* GameScene::createScene()
{
    return GameScene::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool GameScene::init() {
    if (!Scene::init()) { return false; }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto rootNode = CSLoader::createNode("mainscene.csb");
    this->addChild(rootNode); // �[�J�ثe�� scene ��

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("scene101/scene101.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("scene101/scene101bg.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("scene101/gamescene.plist");

    auto loctag = dynamic_cast<cocos2d::Node*>(rootNode->getChildByName("_runner"));
    loctag->setVisible(false);
    auto position = loctag->getPosition();
    _runner = new (std::nothrow) CRunner();
    _runner->init(position, *this, "runner_demo.csb", "collider", 3);

    loctag = dynamic_cast<cocos2d::Sprite*>(rootNode->getChildByName("road00"));
    loctag->setVisible(false);
    position = loctag->getPosition();
    _midObj = new (std::nothrow) CMiddleObject();
    _midObj->init(position, *this, "road00");

    loctag = dynamic_cast<cocos2d::Sprite*>(rootNode->getChildByName("jumpButton"));
    loctag->setVisible(false);
    position = loctag->getPosition();
    _c3sButton = new (std::nothrow)C3SButton();
    _c3sButton->init(_c3sButton->BtnType::jumpBtn, position, *this, "jumpnormal", "jumpon");

    loctag = dynamic_cast<cocos2d::Sprite*>(rootNode->getChildByName("startButton"));
    loctag->setVisible(false);
    position = loctag->getPosition();
    _c3sButton->init(_c3sButton->BtnType::startBtn, position, *this, "startnormal", "starton");

    loctag = dynamic_cast<cocos2d::Sprite*>(rootNode->getChildByName("replaybtn"));
    loctag->setVisible(false);
    position = loctag->getPosition();
    _c3sButton->init(_c3sButton->BtnType::restartBtn, position, *this, "replaybtn", "replaybtn");

    loctag = dynamic_cast<cocos2d::Sprite*>(rootNode->getChildByName("speedButton"));
    loctag->setVisible(false);
    position = loctag->getPosition();
    _c3sButton->init(_c3sButton->BtnType::runBtn, position, *this, "runnormal", "runon");

    _enemy = new (std::nothrow)CEnemy();
    _enemy->init(*this, _runner->getPosition());

    loctag = dynamic_cast<cocos2d::Node*>(rootNode->getChildByName("scoreText"));
    loctag->setVisible(false);
    position = loctag->getPosition();

    CScoring::create(position, dynamic_cast<cocos2d::Sprite*>(rootNode->getChildByName("startButton"))->getPosition(), *this, loctag->getContentSize())->setScore(0);

    _midObj->setSpeed(2);


    //�Ыؤ@�Ӥ@��@���ƥ��ť��
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMoved, this);//�[�JĲ�I���ʨƥ�
    listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);//�[�JĲ�I���}�ƥ�

    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);	//�[�J��Ыت��ƥ��ť��
    this->schedule(CC_SCHEDULE_SELECTOR(GameScene::update));

    return true;
}

void GameScene::update(float dt) {
    if (CScoring::getInstance()->getStart()) {
        if (CScoring::getInstance()->_isInit) {
            CScoring::getInstance()->_isInit = false;
            _c3sButton->setEnable(true);
        }
        _midObj->update(dt);
        _runner->update(dt);
        _c3sButton->setEnable(!_runner->_isJump, _c3sButton->BtnType::jumpBtn);
        _enemy->update(dt);
        if (_runner->_isJump) {
            //CScoring::getInstance()->setMoveSpeed(2.25f);

            if (!_runner->passing) {
                _runner->passing = _enemy->passPlayer(*_runner);
            }
        }
        else {
            //CScoring::getInstance()->setMoveSpeed(2);
        }

        _enemy->setSpeed(CScoring::getInstance()->getMoveSpeed());
        _midObj->setSpeed(CScoring::getInstance()->getMoveSpeed());

        if (_enemy->checkCollider(*_runner)) {
            if (!_runner->_isHurt) {
                _runner->_faceTime = 0;
                _runner->getHurt(_enemy->getHurt());
                _runner->_isHurt = true;
            }
        }
        else {
        }
    }
    else {
        CScoring::getInstance()->update(dt);
        if (!CScoring::getInstance()->_isInit) {
            log("init");
            _runner->initState();
            _enemy->initState();
            _midObj->initState();
            _c3sButton->initState();
            CScoring::getInstance()->initState();
            CScoring::getInstance()->_isInit = true;
        }
    }
}

bool GameScene::onTouchBegan(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)//Ĳ�I�}�l�ƥ�
{
    Point touchLoc = pTouch->getLocation();
    if (_c3sButton->touchesBegin(touchLoc, _c3sButton->BtnType::jumpBtn)) {
        _runner->_isJump = true;
    }

    _c3sButton->touchesBegin(touchLoc);
    return true;
}

void GameScene::onTouchMoved(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)//Ĳ�I�}�l�ƥ�
{
    Point touchLoc = pTouch->getLocation();
    _c3sButton->touchesMove(touchLoc);
}

void GameScene::onTouchEnded(cocos2d::Touch* pTouch, cocos2d::Event* pEvent)//Ĳ�I�}�l�ƥ�
{
    Point touchLoc = pTouch->getLocation();
    _c3sButton->touchesEnd(touchLoc);
}