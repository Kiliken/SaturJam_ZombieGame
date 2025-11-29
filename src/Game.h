# pragma once
# include <Siv3D.hpp>

struct Bullet{
	s3d::Vec2 position;
	s3d::Vec2 direction;

	Bullet(const Vec2& pos, const Vec2& dir)
        : position(pos), direction(dir) {}
};

class Game : public SceneManager<String>::Scene
{
public:

	Game(const InitData& init)
		: IScene{ init }
	{
		Scene::SetBackground(Palette::Dimgray);
		playerTexture = TextureAsset(U"playerTexture");
		enemyTexture = TextureAsset(U"enemyTexture");
		camera = new Camera2D{ Vec2{ 0, 0 } };
	}

	~Game()
	{
		
	}


	void update() override;

	// Draw function
	void draw() const override;

	void drawFadeIn(double t) const override;

	void drawFadeOut(double t) const override;

	Vec2 GenerateEnemy()
	{
		const Circle circle{ Scene::Center(), Scene::Width() / 2 };
		double theta = Random(0.0, 2 * Math::Pi);

		return circle.center + Vec2{ Cos(theta), Sin(theta) } * circle.r;
	}

private:

	const Font font{ FontMethod::MSDF, 48 };

	Camera2D* camera;

	Texture playerTexture;
	Texture enemyTexture;

	Vec2 playerPos{ 400, 500 };
	Array<Vec2> enemies = { GenerateEnemy() };

	s3d::Array<Bullet> playerBullets;

	double PlayerSpeed = 550.0;
	double PlayerBulletSpeed = 500.0;
	double EnemySpeed = 100;
	double EnemyBulletSpeed = 300.0;

	double shootPosDistance = 50.0;
	Vec2 shotPos{ 0, 0 };
    Vec2 aimDirection{ 0, 0 };
    double aimAngle = 0;

	double InitialEnemySpawnInterval = 2.0;
	double enemySpawnTime = InitialEnemySpawnInterval;
	double enemyAccumulatedTime = 0.0;


	double PlayerShotCoolTime = 0.1;
	double playerShotTimer = 0.0;

	double EnemyShotCoolTime = 0.9;
	double enemyShotTimer = 0.0;

	Effect effect;

	int32 highScore = 0;
	int32 score = 0;
};

