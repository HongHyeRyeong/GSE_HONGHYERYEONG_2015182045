#include "stdafx.h"
#include "SceneMgr.h"

SceneMgr::SceneMgr()
{
	m_Renderer = new Renderer(fieldW, fieldH);
	if (!m_Renderer->IsInitialized())
		std::cout << "Renderer could not be initialized.. \n";

	Add(-150, 300, OBJECT_BUILDING, 1);
	Add(0, 300, OBJECT_BUILDING, 1);
	Add(150, 300, OBJECT_BUILDING, 1);
	Add(-150, -300, OBJECT_BUILDING, 2);
	Add(0, -300, OBJECT_BUILDING, 2);
	Add(150, -300, OBJECT_BUILDING, 2);

	for (int i = 0; i < MAX_CHARACTER_COUNT; ++i)
		m_character[i] = NULL;

	for (int i = 0; i < MAX_ATTACK_COUNT; ++i) {
		m_bullet[i] = NULL;
		m_arrow[i] = NULL;
	}

	m_texBuilding1 = m_Renderer->CreatePngTexture("./Resource/pizza.png");
	m_texBuilding2 = m_Renderer->CreatePngTexture("./Resource/chicken.png");
	m_texChara1 = m_Renderer->CreatePngTexture("./Resource/pizza2.png");
	m_texChara2 = m_Renderer->CreatePngTexture("./Resource/chicken2.png");

	northTime = 0;
	southTime = 7;
}

SceneMgr::~SceneMgr()
{
	for (int i = 0; i < 6; ++i)
		if (m_building != NULL)
			delete m_building[i];

	for (int i = 0; i < MAX_CHARACTER_COUNT; ++i)
		if (m_character[i] != NULL)
			delete m_character[i];

	for (int i = 0; i < MAX_ATTACK_COUNT; ++i)
		if (m_bullet[i] != NULL)
			delete m_bullet[i];

	for (int i = 0; i < MAX_ATTACK_COUNT; ++i)
		if (m_arrow[i] != NULL)
			delete m_arrow[i];

	delete m_Renderer;
	delete[] m_building;
	delete[] m_character;
	delete[] m_bullet;
	delete[] m_arrow;
}

void SceneMgr::Add(float x, float y, int type, int id)
{
	if (type == OBJECT_BUILDING) {
		for (int i = 0; i < 6; ++i)
			if (m_building[i] == NULL) {
				m_building[i] = new Object(x, y, type, id);
				break;
			}
	}
	else if (type == OBJECT_CHARACTER) {
		for (int i = 0; i < MAX_CHARACTER_COUNT; ++i)
			if (m_character[i] == NULL) {
				m_character[i] = new Object(x, y, type, id);

				if (id == 2)
					southTime = 0;

				break;
			}
	}
	else if (type == OBJECT_BULLET) {
		for (int i = 0; i < MAX_ATTACK_COUNT; ++i)
			if (m_bullet[i] == NULL) {
				m_bullet[i] = new Object(x, y, type, id);
				break;
			}
	}
	else if (type == OBJECT_ARROW) {
		for (int i = 0; i < MAX_ATTACK_COUNT; ++i)
			if (m_arrow[i] == NULL) {
				m_arrow[i] = new Object(x, y, type, id);
				break;
			}
	}
}

void SceneMgr::Update(float elapsedTime)
{
	// 유닛 배치
	northTime += elapsedTime / 1000;
	southTime += elapsedTime / 1000;

	if (northTime >= 5) {
		random_device rd;
		mt19937_64 rng(rd());
		uniform_int_distribution<> uiX(-200, 200);
		uniform_int_distribution<> uiY(150, 300);

		Add(uiX(rng), uiY(rng), OBJECT_CHARACTER, 1);
		northTime = 0;
	}

	// 오브젝트 업데이트
	for (int i = 0; i < 6; ++i)
		if (m_building[i] != NULL) {
			m_building[i]->Update(elapsedTime);

			if (m_building[i]->getAttackTime() >= 10) {
				Add(m_building[i]->getX(), m_building[i]->getY(), OBJECT_BULLET, m_building[i]->getId());
				m_building[i]->setAttackTime(0);
			}
		}

	for (int i = 0; i < MAX_CHARACTER_COUNT; ++i)
		if (m_character[i] != NULL) {
			m_character[i]->Update(elapsedTime);

			if (m_character[i]->getAttackTime() >= 3) {
				Add(m_character[i]->getX(), m_character[i]->getY(), OBJECT_ARROW, m_character[i]->getId());
				m_character[i]->setAttackTime(0);
			}
		}

	for (int i = 0; i < MAX_ATTACK_COUNT; ++i)
		if (m_bullet[i] != NULL)
			m_bullet[i]->Update(elapsedTime);

	for (int i = 0; i < MAX_ATTACK_COUNT; ++i)
		if (m_arrow[i] != NULL)
			m_arrow[i]->Update(elapsedTime);

	// 충돌체크
	Collision();

	// 소멸
	for (int i = 0; i < 6; ++i)
		if (m_building[i] != NULL&&m_building[i]->getLife() <= 0) {
			delete m_building[i];
			m_building[i] = NULL;
		}

	for (int i = 0; i < MAX_CHARACTER_COUNT; ++i) {
		if (m_character[i] != NULL && m_character[i]->getLife() <= 0) {
			delete m_character[i];
			m_character[i] = NULL;
		}
	}

	for (int i = 0; i < MAX_ATTACK_COUNT; ++i) {
		if (m_bullet[i] != NULL && m_bullet[i]->getLife() <= 0) {
			delete m_bullet[i];
			m_bullet[i] = NULL;
		}
	}

	for (int i = 0; i < MAX_ATTACK_COUNT; ++i) {
		if (m_arrow[i] != NULL && m_arrow[i]->getLife() <= 0) {
			delete m_arrow[i];
			m_arrow[i] = NULL;
		}
	}
}

void SceneMgr::DrawSolidRect()
{
	for (int i = 0; i < 6; ++i)
		if (m_building[i] != NULL) {
			if (m_building[i]->getId() == 1) {
				m_Renderer->DrawTexturedRect(m_building[i]->getX(), m_building[i]->getY(), 0,
					m_building[i]->getSize(), m_building[i]->getRGB(0), m_building[i]->getRGB(1), m_building[i]->getRGB(2), 1, m_texBuilding1, 0.1);
				m_Renderer->DrawSolidRectGauge(m_building[i]->getX(), m_building[i]->getY() + 60, 0, 100, 5, 1, 0, 0, 1, m_building[i]->getLife() / 500, 0.1);

			}
			else {
				m_Renderer->DrawTexturedRect(m_building[i]->getX(), m_building[i]->getY(), 0,
					m_building[i]->getSize(), m_building[i]->getRGB(0), m_building[i]->getRGB(1), m_building[i]->getRGB(2), 1, m_texBuilding2, 0.1);
				m_Renderer->DrawSolidRectGauge(m_building[i]->getX(), m_building[i]->getY() + 60, 0, 100, 5, 0, 0, 1, 1, m_building[i]->getLife() / 500, 0.1);
			}
		}

	for (int i = 0; i < MAX_CHARACTER_COUNT; ++i)
		if (m_character[i] != NULL) {
			if (m_character[i]->getId() == 1) {
				m_Renderer->DrawTexturedRect(m_character[i]->getX(), m_character[i]->getY(), 0,
					m_character[i]->getSize(), m_character[i]->getRGB(0), m_character[i]->getRGB(1), m_character[i]->getRGB(2), 1, m_texChara1, 0.2);
				m_Renderer->DrawSolidRectGauge(m_character[i]->getX(), m_character[i]->getY() + 20, 0, 40, 5, 1, 0, 0, 1, m_character[i]->getLife() / 100, 0.2);
			}
			else {
				m_Renderer->DrawTexturedRect(m_character[i]->getX(), m_character[i]->getY(), 0,
					m_character[i]->getSize(), m_character[i]->getRGB(0), m_character[i]->getRGB(1), m_character[i]->getRGB(2), 1, m_texChara2, 0.2);
				m_Renderer->DrawSolidRectGauge(m_character[i]->getX(), m_character[i]->getY() + 20, 0, 40, 5, 0, 0, 1, 1, m_character[i]->getLife() / 100, 0.2);
			}
		}

	for (int i = 0; i < MAX_ATTACK_COUNT; ++i)
		if (m_bullet[i] != NULL)
			m_Renderer->DrawSolidRect(m_bullet[i]->getX(), m_bullet[i]->getY(), 0,
				m_bullet[i]->getSize(), m_bullet[i]->getRGB(0), m_bullet[i]->getRGB(1), m_bullet[i]->getRGB(2), 1, 0.3);

	for (int i = 0; i < MAX_ATTACK_COUNT; ++i)
		if (m_arrow[i] != NULL)
			m_Renderer->DrawSolidRect(m_arrow[i]->getX(), m_arrow[i]->getY(), 0,
				m_arrow[i]->getSize(), m_arrow[i]->getRGB(0), m_arrow[i]->getRGB(1), m_arrow[i]->getRGB(2), 1, 0.3);
}

void SceneMgr::Collision()
{
	// 빌딩과 캐릭터
	for (int i = 0; i < 6; ++i)
		if (m_building[i] != NULL)
			for (int j = 0; j < MAX_CHARACTER_COUNT; ++j)
				if (m_character[j] != NULL&&m_building[i]->getId() != m_character[j]->getId())
				{
					float size = m_building[i]->getSize() / 2;
					float size1 = m_character[j]->getSize() / 2;

					if (CollisionTest(m_building[i]->getX() - size, m_building[i]->getY() - size, m_building[i]->getX() + size, m_building[i]->getY() + size,
						m_character[j]->getX() - size1, m_character[j]->getY() - size1, m_character[j]->getX() + size1, m_character[j]->getY() + size1))
					{
						m_building[i]->Collision(m_character[j]->getLife());
						delete m_character[j];
						m_character[j] = NULL;
					}
				}

	// 빌딩과 총알
	for (int i = 0; i < 6; ++i)
		if (m_building[i] != NULL)
			for (int j = 0; j < MAX_ATTACK_COUNT; ++j)
				if (m_bullet[j] != NULL&&m_building[i]->getId() != m_bullet[j]->getId())
				{
					float size = m_building[i]->getSize() / 2;
					float size1 = m_bullet[j]->getSize() / 2;

					if (CollisionTest(m_building[i]->getX() - size, m_building[i]->getY() - size, m_building[i]->getX() + size, m_building[i]->getY() + size,
						m_bullet[j]->getX() - size1, m_bullet[j]->getY() - size1, m_bullet[j]->getX() + size1, m_bullet[j]->getY() + size1))
					{
						m_building[i]->Collision(m_bullet[j]->getLife());
						delete m_bullet[j];
						m_bullet[j] = NULL;
					}
				}

	// 캐릭터와 총알
	for (int i = 0; i < MAX_CHARACTER_COUNT; ++i)
		if (m_character[i] != NULL)
			for (int j = 0; j < MAX_ATTACK_COUNT; ++j)
				if (m_bullet[j] != NULL&&m_character[i]->getId() != m_bullet[j]->getId())
				{
					float size = m_character[i]->getSize() / 2;
					float size1 = m_bullet[j]->getSize() / 2;

					if (CollisionTest(m_character[i]->getX() - size, m_character[i]->getY() - size, m_character[i]->getX() + size, m_character[i]->getY() + size,
						m_bullet[j]->getX() - size1, m_bullet[j]->getY() - size1, m_bullet[j]->getX() + size1, m_bullet[j]->getY() + size1))
					{
						m_character[i]->Collision(m_bullet[j]->getLife());
						delete m_bullet[j];
						m_bullet[j] = NULL;
					}
				}

	// 빌딩과 화살
	for (int i = 0; i < 6; ++i)
		if (m_building[i] != NULL)
			for (int j = 0; j < MAX_ATTACK_COUNT; ++j)
				if (m_arrow[j] != NULL&&m_building[i]->getId() != m_arrow[j]->getId())
				{
					float size = m_building[i]->getSize() / 2;
					float size1 = m_arrow[j]->getSize() / 2;

					if (CollisionTest(m_building[i]->getX() - size, m_building[i]->getY() - size, m_building[i]->getX() + size, m_building[i]->getY() + size,
						m_arrow[j]->getX() - size1, m_arrow[j]->getY() - size1, m_arrow[j]->getX() + size1, m_arrow[j]->getY() + size1))
					{
						m_building[i]->Collision(m_arrow[j]->getLife());
						delete m_arrow[j];
						m_arrow[j] = NULL;
					}
				}

	// 캐릭터와 화살
	for (int i = 0; i < MAX_CHARACTER_COUNT; ++i)
		if (m_character[i] != NULL)
			for (int j = 0; j < 100; ++j)
				if (m_arrow[j] != NULL&&m_character[i]->getId() != m_arrow[j]->getId())
				{
					float size = m_character[i]->getSize() / 2;
					float size1 = m_arrow[j]->getSize() / 2;

					if (CollisionTest(m_character[i]->getX() - size, m_character[i]->getY() - size, m_character[i]->getX() + size, m_character[i]->getY() + size,
						m_arrow[j]->getX() - size1, m_arrow[j]->getY() - size1, m_arrow[j]->getX() + size1, m_arrow[j]->getY() + size1))
					{
						m_character[i]->Collision(m_arrow[j]->getLife());
						delete m_arrow[j];
						m_arrow[j] = NULL;
					}
				}
}

bool SceneMgr::CollisionTest(float left, float bottom, float right, float top, float left1, float bottom1, float right1, float top1)
{
	if (left > right1)
		return false;
	if (right < left1)
		return false;
	if (bottom > top1)
		return false;
	if (top < bottom1)
		return false;

	return true;
}