// 次世代湍流 宇宙无敌

#ifndef __cube3d__BuildingScene__
#define __cube3d__BuildingScene__

#include <vector>
#include <string>
#include <unordered_map>

#include "cocos2d.h"
#include "TRBaseScene.h"
#include "BrushLayer.h"
#include "CubeSprite.h"


//template <class T>
//inline void hash_combine(std::size_t& seed, const T& v)
//{
//    std::hash<T> hasher;
//    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
//}
//
//struct Vec3Hasher
//{
//    std::size_t operator()(const cocos2d::Vec3& k) const
//    {
//        std::size_t seed = 0;
//        hash_combine(seed, k.x);
//        hash_combine(seed, k.y);
//        hash_combine(seed, k.z);
//        return seed;
//    }
//};

class BuildingScene: public TRBaseScene
{
public:
    virtual bool init() override;
    CREATE_FUNC(BuildingScene);

    enum FaceType
    {
        FXP, // x positive
        FXN, // x negative
        FYP,
        FYN,
        FZP,
        FZN
    };
private:
    enum UnrealType
    {
        UX,UY,UZ
    };
    const float _cubeWidth = 10;

    void reloadMetaCubes();
    void copyTemplateWorkspace();

    void initMenuButtons();

    BrushLayer* _brushLayer;

    cocos2d::Layer* _sceneLayer;
    cocos2d::Camera* _sceneCamera;
    void initSceneLayer();

    // Core data
    // TODO 可能需要X叉树进行空间管理以加快搜索。
    // MORE 现在使用hashTable应该能够达到较快的搜索效率了，空间换时间。
//    std::unordered_map<cocos2d::Vec3, CubeSprite*, Vec3Hasher> _cubeMap;//有bug
    std::vector<CubeSprite*> _cubes;
    // 增加一个Cube到Scene中，会加入到_sceneLayer和_cubeMap。
    void addCube(CubeSprite* cube);
    // 从_sceneLayer和_cubeMap中去掉一个Cube。
    void removeCube(CubeSprite* cube);
    cocos2d::Vec3 rawPos2Real(cocos2d::Vec3 rawPos);

    void addUnrealWall(UnrealType t, int width);

    // 漫游
    void update(float dt)override;
    bool _zoomIn = false;
    bool _zoomOut = false;
    bool _left = false;
    bool _right = false;
    bool _up = false;
    bool _down = false;
    bool _isFPS = false;
    bool _front = false;
    bool _back = false;
    bool _rotateUp = false;
    bool _rotateDown = false;
    bool _rotateLeft = false;
    bool _rotateRight = false;
    cocos2d::Vec2 _fpsAnchor;
    float _rotateY = 0.f;
    float _rotateX = 0.f;


    const float MOVE_SCALE = 1.f;
    const float UP_DOWN_MAX = 0.99f;
    const float ROTATE_SCALE = 0.005f;
    const float SCALL_MOVE_SCALE = 1.f;
    const float ROTATE_KEY_STEP = 1.f;
    const float PI = 3.1415926f;

    // 找到选择的cube
    void calcIntersection();
    CubeSprite* getIntersection(int* face);
    CubeSprite* _lastSelected = nullptr;
    int _lastSelectedFace = 0;
    void addAnCubeAlignSelectedFace();
    void deleteTheSelectedCube();
};

#endif /* defined(__cube3d__BuildingScene__) */
