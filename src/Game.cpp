# include "Game.h"


void Game::drawFadeIn(double t) const
{
	draw();
	Circle{ Scene::Size().x / 2, Scene::Size().y / 2, Scene::Size().x }
	.drawFrame(((1 - t) * Scene::Size().x), 0, ColorF{ 0.2, 0.3, 0.4 });



}

void Game::drawFadeOut(double t) const
{
	draw();
	Circle{ Scene::Size().x / 2, Scene::Size().y / 2, Scene::Size().x }
	.drawFrame((t * Scene::Size().x), 0, ColorF{ 0.2, 0.3, 0.4 });
}

void Game::update()
{
	const uint64 t = Time::GetMillisec();
	const int32 x = (t / 100 % 36);


	// Game over check
	bool gameover = false;

	const double deltaTime = Scene::DeltaTime();
	enemyAccumulatedTime += deltaTime;
	playerShotTimer = Min((playerShotTimer + deltaTime), PlayerShotCoolTime);
	enemyShotTimer += deltaTime;

	// Generate enemies
	while (enemySpawnTime <= enemyAccumulatedTime)
	{
		enemyAccumulatedTime -= enemySpawnTime;
		enemySpawnTime = Max(enemySpawnTime * 0.95, 0.3);
		enemies << GenerateEnemy();
	}

	// Player movement
	const Vec2 move = Vec2{ (KeyD.pressed() - KeyA.pressed()), (KeyS.pressed() - KeyW.pressed()) }
	.setLength(deltaTime * PlayerSpeed * (KeyShift.pressed() ? 1.0 : 0.5));
	playerPos.moveBy(move).clamp(Scene::Rect());

	//CrossHair
	Vec2 cursorPos = Cursor::PosF();
	aimDirection = (cursorPos - playerPos).normalized();
	aimAngle = Math::Atan2(aimDirection.x * -1, aimDirection.y);
	shotPos = playerPos + (aimDirection * shootPosDistance);

	// Player shot firing
	if (PlayerShotCoolTime <= playerShotTimer && KeySpace.pressed())
	{
		playerShotTimer -= PlayerShotCoolTime;
		playerBullets << Bullet(shotPos, aimDirection);
	}

	// Move player shots
	for (size_t i = 0; i < playerBullets.size(); ++i) {
		playerBullets[i].position += playerBullets[i].direction * (deltaTime * PlayerBulletSpeed);
	}


	// Remove player shots that went off screen
	playerBullets.remove_if([](const Bullet& b) {
		return (b.position.y < -20 || b.position.y > Scene::Height() + 20 || b.position.x < -20 || b.position.x > Scene::Width() + 20);
	});

	// Move enemies
	for (auto& enemy : enemies)
	{
		float dx = playerPos.x - enemy.x;
		float dy = playerPos.y - enemy.y;

		// Calculate distance
		float distance = enemy.distanceFrom(playerPos);

		// Avoid division by zero
		if (distance > 0.0f)
		{
			// Normalize direction
			float dirX = dx / distance;
			float dirY = dy / distance;

			// Move enemy at constant speed
			enemy.x += dirX * EnemySpeed * deltaTime;
			enemy.y += dirY * EnemySpeed * deltaTime;
		}
	}


	// Remove enemies that went off screen
	enemies.remove_if([&](const Vec2& e)
	{
		if (700 < e.y)
		{
			// Game over if enemy goes off screen
			gameover = true;
			return true;
		}
		else
		{
			return false;
		}
	});



	////////////////////////////////
	//
	//	Hit detection
	//
	////////////////////////////////

	for (auto itEnemy = enemies.begin(); itEnemy != enemies.end();)
	{
		const Circle enemyCircle{ *itEnemy, 40 };
		bool skip = false;

		for (auto itBullet = playerBullets.begin(); itBullet != playerBullets.end();)
		{
			if (enemyCircle.intersects(itBullet->position))
			{
				// Add explosion effect
				effect.add([pos = *itEnemy](double t)
				{
					const double t2 = ((0.5 - t) * 2.0);
					Circle{ pos, (10 + t * 280) }.drawFrame((20 * t2), ColorF{ 1.0, (t2 * 0.5) });
					return (t < 0.5);
				});

				itEnemy = enemies.erase(itEnemy);
				playerBullets.erase(itBullet);
				++score;
				skip = true;
				break;
			}

			++itBullet;
		}

		if (skip)
		{
			continue;
		}

		++itEnemy;
	}


	// Enemy vs player
	for (auto& enemy : enemies)
	{
		if (enemy.distanceFrom(playerPos) <= 20)
		{
			// Game over
			gameover = true;
			break;
		}
	}

	// Reset if game over
	if (gameover)
	{
		changeScene(U"Title", 1.5s);
	}
}

void Game::draw() const
{
	camera->update();
	{
		//Scene::SetBackground(ColorF(0.0, 0.6, 0.4));
		const uint64 t = Time::GetMillisec();
		const int32 x = (t / 100 % 36);


		// Draw player
		playerTexture.resized(80).drawAt(playerPos);

		// Draw CrossHair
		Circle{ shotPos, 8 }.draw(Palette::Red);

		// Draw player shots
		for (const auto& playerBullet : playerBullets)
		{
			Circle{ playerBullet.position, 8 }.draw(Palette::Orange);
		}

		// Draw enemies
		for (const auto& enemy : enemies)
		{
			enemyTexture.resized(60).drawAt(enemy);
		}

		// Draw explosion effects
		effect.update();

		// Draw score
		font(U"[{}]"_fmt(score)).draw(30, Arg::bottomRight(780, 580));
	}
}
