#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(FlappyPlayerObject, PlayerObject) {
	struct Fields{
		bool isDownRotated = false;
		float jumpRot = -30.f;
		float fallRot = 60.f;
	};

	void cooldownRot(float dt){
		m_fields->isDownRotated = true;
	}

	void update(float p0) {
		PlayerObject::update(p0);

		auto fields = m_fields.self();
		if (!m_isBird) return;

		if (m_fields->isDownRotated && PlayLayer::get()) {
			this->setRotation(fields->fallRot);
		}

	}

	void hitGround(GameObject* p0, bool p1) {
		PlayerObject::hitGround(p0, p1);

		if (!m_isBird) return;

		if(PlayLayer::get()) {
			this->stopAllActions();
			this->unscheduleAllSelectors();
			m_fields->isDownRotated = false;
		}
		
	}

	bool pushButton(PlayerButton p0) {
		if (!PlayerObject::pushButton(p0)) return false;

		auto fields = m_fields.self();

		if (!m_isBird) return true;

		float jumpAnimTime = 0.1f / m_gravityMod;
		float fallAnimTime = 0.5f / m_gravityMod;

		if (PlayLayer::get()) {
			this->stopAllActions();
			fields->isDownRotated = false;
			this->unscheduleAllSelectors();

			if (m_isSecondPlayer) {
				if (m_isUpsideDown) {
					fields->jumpRot = 30.f;
					fields->fallRot = -60.f;
				} else {
					fields->jumpRot = -30.f;
					fields->fallRot = 60.f;
				}
			} else if (!m_isSecondPlayer) {
				if (!m_isUpsideDown) {
					fields->jumpRot = -30.f;
					fields->fallRot = 60.f;
				} else {
					fields->jumpRot = 30.f;
					fields->fallRot = -60.f;
				}
			}

			if (m_isGoingLeft) {
				fields->jumpRot = -fields->jumpRot;
				fields->fallRot = -fields->fallRot;
			} else {
				fields->jumpRot = fields->jumpRot;
				fields->fallRot = fields->fallRot;
			}

			auto justJumpedAnim = CCEaseOut::create(CCRotateTo::create(jumpAnimTime, fields->jumpRot), 2.0f);
			auto theRotatingDownAnimThingLol = CCEaseIn::create(CCRotateTo::create(fallAnimTime, fields->fallRot), 2.0f);

			auto bothLol = CCSequence::create(
				justJumpedAnim,
				theRotatingDownAnimThingLol,
				nullptr
			);

			this->runAction(bothLol);
			this->scheduleOnce(schedule_selector(FlappyPlayerObject::cooldownRot), jumpAnimTime + fallAnimTime);
		}
		return true;

	}
};

class $modify(FlappyPlayLayer, PlayLayer) {
	void resetLevel() {
		PlayLayer::resetLevel();

		m_player1->stopAllActions();
		m_player1->unscheduleAllSelectors();
		m_player2->stopAllActions();
		m_player2->unscheduleAllSelectors();
	}
};