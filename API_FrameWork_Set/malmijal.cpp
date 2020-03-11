#include "stdafx.h"
#include "malmijal.h"


malmijal::malmijal()
{
}


malmijal::~malmijal()
{
}

HRESULT malmijal::init()
{
	_positionX = 0;					//������ �߰���ġ
	_positionY = 0;					//������ �߰���ġ
	_positionZ = 0;					//������ ����


	_monsterFrameCount = 0;			//����
	_currentFrameX = 0;				//�������̹��� x�� ��
	_currentFrameY = 0;				//�������̹��� y�� ��

	_enemyAngle = 0;				//������ ����
	_enemySpeed = 0;				//������ �ӵ�

	_rcAlpha = 255;
	_rcAlphaTime = 0;

	_enemyDirection = ENEMY_IDLE;

	sprintf_s(_name, "�ٺ�");		//������ �̸�

	_level = 0;					//������ ����
	_hp = 0;
	_hpMax = 0;
	_mp = 0;
	_mpMax = 0;
	_atk = 0;
	_def = 0;

	_isFrameImage = false;				//���� �������̹��� ����
	_isMove = false;					//�������� ������ ���°�
	_isDie = false;					//���翩�� ���°�
	_isStunAtk = false;
	_isStun = false;

	_stunTime = 0;
	startTime = 0;
	stopTime = 0;

	return S_OK;
}

void malmijal::setEnemy(POINT position)
{
	_img = new image;
	_img->init("images/enemy/malmijal.bmp", 144, 48, 3, 1, true, RGB(255, 0, 255));
	_positionX = position.x;
	_positionY = position.y;
	_rc = RectMake(_positionX, _positionY, 20, 20);
	_hp = 2;
	_atk = 1;
	_enemySpeed = 2.0f;
}

void malmijal::release()
{
}

void malmijal::update()
{
	curTileMap = SCENEMANAGER->getCurrentTileMap();

	if (_isKnockBack)
	{
		_rc = RectMakeCenter(_positionX, _positionY, 0, 0);
	}
	else
	{
		_rc = RectMakeCenter(_positionX, _positionY, 20, 20);
	}

	if (!_isStun)
	{
		//���ʹ� ����
		heading();
		animation();

		if (_isMove)
		{
			float positionTempX = _positionX + cosf(_enemyAngle) * _enemySpeed;
			float positionTempY = _positionY + -sinf(_enemyAngle) * _enemySpeed;

			RECT garbegeRect, rectTemp = RectMakeCenter(positionTempX, positionTempY, _rc.right - _rc.left, _rc.bottom - _rc.top);
			vector<tagInteractionObject> interObjTemp = curTileMap.getInteractionObject();

			for (int i = 0; i < interObjTemp.size(); i++)
			{
				if (interObjTemp[i].xPos*TILESIZE + interObjTemp[i].widthPos*TILESIZE< _player.getCameraX() || interObjTemp[i].xPos*TILESIZE>_player.getCameraX() + WINSIZEX)
				{
					if (!(i == interObjTemp.size() - 1))
					{
						continue;
					}
				}
				else if (interObjTemp[i].yPos*TILESIZE + interObjTemp[i].heightPos*TILESIZE< _player.getCameraY() || interObjTemp[i].yPos*TILESIZE>_player.getCameraY() + WINSIZEY)
				{
					if (!(i == interObjTemp.size() - 1))
					{
						continue;
					}
				}

				if (IntersectRect(&garbegeRect, &interObjTemp[i]._rc, &rectTemp))
				{
					return;
				}
				if (i == interObjTemp.size() - 1)
				{
					_positionX += cosf(_enemyAngle)*_enemySpeed;
					_positionY += -sinf(_enemyAngle)*_enemySpeed;
				}
			}

		}

		move();

		stop();

		//3�ʸ��� ������ ����
		if (GetTickCount() - startTime > 4000)
		{
			_isMove = true;
			startTime = GetTickCount();
			int tempMove = RND->getInt(7);
			switch (tempMove)
			{
			case 0:
				_enemyAngle = getAngle(_positionX, _positionY, _positionX, _positionY - 200);
				destination = PointMake(_positionX, _positionY - 100);
				break;
			case 1:
				_enemyAngle = getAngle(_positionX, _positionY, _positionX, _positionY + 200);
				destination = PointMake(_positionX, _positionY + 100);
				break;
			case 2:
				_enemyAngle = getAngle(_positionX, _positionY, _positionX - 200, _positionY);
				destination = PointMake(_positionX - 100, _positionY);
				break;
			case 3:
				_enemyAngle = getAngle(_positionX, _positionY, _positionX + 200, _positionY);
				destination = PointMake(_positionX + 100, _positionY);
				break;
			case 4:
				_enemyAngle = getAngle(_positionX, _positionY, _positionX + 200, _positionY - 200);
				destination = PointMake(_positionX + 60, _positionY - 60);
				break;
			case 5:
				_enemyAngle = getAngle(_positionX, _positionY, _positionX + 200, _positionY + 200);
				destination = PointMake(_positionX + 60, _positionY + 60);
				break;
			case 6:
				_enemyAngle = getAngle(_positionX, _positionY, _positionX - 200, _positionY - 200);
				destination = PointMake(_positionX - 120, _positionY - 120);
				break;
			case 7:
				_enemyAngle = getAngle(_positionX, _positionY, _positionX - 200, _positionY - 200);
				destination = PointMake(_positionX - 60, _positionY - 60);
				break;
			default:
				break;
			}
		}

		//�������� �����ϸ� ��� ����
		if (getDistance(_positionX, _positionY, destination.x, destination.y) < 10)
		{
			_isMove = false;
		}
	}


	collision();

}

void malmijal::render(HDC hdc)
{
	//_img->frameRender(hdc, _rc.left - 14, _rc.top - 14, _img->getFrameX(), _img->getFrameY());
	_img->alphaRender(hdc, _positionX - _img->getFrameWidth() / 2, _positionY - _img->getFrameHeight() / 2, _img->getFrameX() * 48, _img->getFrameY() * 48, 48, 48, _rcAlpha);
	if (_isDebug)
	{
		RectangleMakeCenter(getMemDC(), _positionX, _positionY, 20, 20);
	}

}

void malmijal::animation()
{
	_monsterFrameCount++;
	_img->setFrameY(0);
	if (_monsterFrameCount % 8 == 0)
	{
		_monsterFrameCount = 0;
		_img->setFrameX(_img->getMaxFrameX() - _monsterFrameIndex++);
		if (_monsterFrameIndex > _img->getMaxFrameX()) _monsterFrameIndex = 0;
	}

}

void malmijal::collision()
{
	if (_isKnockBack)
	{
		_enemyAngle = getAngle(_positionX, _positionY, _player.getPositionCenterX(), _player.getPositionCenterY());

		_rcAlphaTime += 1;
		if (_rcAlphaTime % 4 == 0)
		{
			_rcAlpha = 0;
		}
		else
		{
			_rcAlpha = 255;
		}

		_isMove = false;


		if (getDistance(_postPositionX, _postPositionY, _positionX, _positionY) <= 150)
		{
			float positionTempX = _positionX - cosf(_enemyAngle) * 15;
			float positionTempY = _positionY + sinf(_enemyAngle) * 15;

			RECT garbegeRect,rectTemp = RectMakeCenter(positionTempX, positionTempY, _rc.right - _rc.left, _rc.bottom - _rc.top);

			if (curTileMap.getMapObj((rectTemp.left - 10) / 24, (rectTemp.top - 10) / 24) != OBJ_BLOCK &&
				curTileMap.getMapObj((rectTemp.right + 10) / 24, (rectTemp.top - 10) / 24) != OBJ_BLOCK &&
				curTileMap.getMapObj((rectTemp.left - 10) / 24, (rectTemp.bottom + 10) / 24) != OBJ_BLOCK &&
				curTileMap.getMapObj((rectTemp.right + 10) / 24, (rectTemp.bottom + 10) / 24) != OBJ_BLOCK)
			{
				vector<tagInteractionObject> interObjTemp = curTileMap.getInteractionObject();

				for (int i = 0; i < interObjTemp.size(); i++)
				{
					if (interObjTemp[i].xPos*TILESIZE + interObjTemp[i].widthPos*TILESIZE< _player.getCameraX() || interObjTemp[i].xPos*TILESIZE>_player.getCameraX() + WINSIZEX)
					{
						if (!(i == interObjTemp.size() - 1))
						{
							continue;
						}
					}
					else if (interObjTemp[i].yPos*TILESIZE + interObjTemp[i].heightPos*TILESIZE< _player.getCameraY() || interObjTemp[i].yPos*TILESIZE>_player.getCameraY() + WINSIZEY)
					{
						if (!(i == interObjTemp.size() - 1))
						{
							continue;
						}
					}

					if (IntersectRect(&garbegeRect, &interObjTemp[i]._rc, &rectTemp))
					{
						break;
					}
					if (i == interObjTemp.size() - 1)
					{
						_positionX = positionTempX;
						_positionY = positionTempY;
					}
				}
			}
		}

		if (_rcAlphaTime >= 50)
		{
			_rcAlpha = 255;
			_isKnockBack = false;
			_rcAlphaTime = 0;
			if (_isStunAtk && !_isStun)
			{
				_isStun = true;
			}
		}

	}
	else if (!_isKnockBack)
	{
		_postPositionX = _positionX;
		_postPositionY = _positionY;
	}

	if (_isStun)
	{
		_stunTime += 1;

		if (_stunTime >= 150 && _stunTime % 5 == 1)
		{
			_positionX -= 4;
		}
		else if (_stunTime >= 150 && _stunTime % 5 == 0)
		{
			_positionX += 4;
		}

		if (_stunTime >= 200)
		{
			_isStun = false;
			_stunTime = 0;
		}
	}
}

void malmijal::move()
{
	if (_isMove)
	{
		if (_checkDirection == ENEMY_UP)
		{
			if (curTileMap.getMapObj(_positionX / 24, _rc.top / 24) == OBJ_BLOCK ||
				curTileMap.getMapObj(_rc.left / 24, _rc.top / 24) == OBJ_BLOCK ||
				curTileMap.getMapObj(_rc.right / 24, _rc.top / 24) == OBJ_BLOCK)
			{
				_positionY += 1;
				_positionY += -sinf(PI2 - _enemyAngle)*(_enemySpeed);
			}
			if (curTileMap.getMapObj(_rc.left / 24, _positionY / 24) == OBJ_BLOCK)
			{
				_positionX += 1;
				_positionX += cosf(PI - _enemyAngle)*(_enemySpeed);
			}
			if (curTileMap.getMapObj(_rc.right / 24, _positionY / 24) == OBJ_BLOCK)
			{
				_positionX -= 1;
				_positionX += cosf(PI - _enemyAngle)*(_enemySpeed);
			}
		}

		if (_checkDirection == ENEMY_DOWN)
		{
			if (curTileMap.getMapObj(_positionX / 24, _rc.bottom / 24) == OBJ_BLOCK ||
				curTileMap.getMapObj(_rc.left / 24, _rc.bottom / 24) == OBJ_BLOCK ||
				curTileMap.getMapObj(_rc.right / 24, _rc.bottom / 24) == OBJ_BLOCK)
			{
				_positionY -= 1;
				_positionY += -sinf(PI2 - _enemyAngle)*(_enemySpeed);
			}

			if (curTileMap.getMapObj(_rc.left / 24, _positionY / 24) == OBJ_BLOCK)
			{
				_positionX += 1;
				_positionX += cosf(PI - _enemyAngle)*(_enemySpeed);
			}
			if (curTileMap.getMapObj(_rc.right / 24, _positionY / 24) == OBJ_BLOCK)
			{
				_positionX -= 1;
				_positionX += cosf(PI - _enemyAngle)*(_enemySpeed);
			}
		}

		if (_checkDirection == ENEMY_LEFT)
		{
			if (curTileMap.getMapObj(_rc.left / 24, _rc.top / 24) == OBJ_BLOCK ||
				curTileMap.getMapObj(_rc.left / 24, _positionY / 24) == OBJ_BLOCK ||
				curTileMap.getMapObj(_rc.left / 24, _rc.bottom / 24) == OBJ_BLOCK)
			{
				_positionX += 1;
				_positionX += cosf(PI - _enemyAngle)*(_enemySpeed);
			}

			if (curTileMap.getMapObj(_positionX / 24, _rc.top / 24) == OBJ_BLOCK)
			{
				_positionY += 1;
				_positionY += -sinf(PI2 - _enemyAngle)*(_enemySpeed);
			}
			if (curTileMap.getMapObj(_positionX / 24, _rc.bottom / 24) == OBJ_BLOCK)
			{
				_positionY -= 1;
				_positionY += -sinf(PI2 - _enemyAngle)*(_enemySpeed);
			}
		}

		if (_checkDirection == ENEMY_RIGHT)
		{
			if (curTileMap.getMapObj(_rc.right / 24, _rc.top / 24) == OBJ_BLOCK ||
				curTileMap.getMapObj(_rc.right / 24, _positionY / 24) == OBJ_BLOCK ||
				curTileMap.getMapObj(_rc.right / 24, _rc.bottom / 24) == OBJ_BLOCK)
			{
				_positionX -= 1;
				_positionX += cosf(PI - _enemyAngle)*(_enemySpeed);
			}

			if (curTileMap.getMapObj(_positionX / 24, _rc.top / 24) == OBJ_BLOCK)
			{
				_positionY += 1;
				_positionY += -sinf(PI2 - _enemyAngle)*(_enemySpeed);
			}
			if (curTileMap.getMapObj(_positionX / 24, _rc.bottom / 24) == OBJ_BLOCK)
			{
				_positionY -= 1;
				_positionY += -sinf(PI2 - _enemyAngle)*(_enemySpeed);
			}
		}

	}

	if (_rc.right + 10 >= curTileMap.getMapSizeX() * 24)
	{
		_positionX -= 10;
	}
	if (_rc.left - 10 <= MAPSTARTVIEWX)
	{
		_positionX += 10;
	}
	if (_rc.bottom + 10 >= curTileMap.getMapSizeY() * 24)
	{
		_positionY -= 10;
	}
	if (_rc.top - 10 <= MAPSTARTVIEWY)
	{
		_positionY += 10;
	}
}

void malmijal::heading()
{
	if (_enemyAngle >= PI / 4 && _enemyAngle <= 3 * PI / 4)
	{
		_checkDirection = ENEMY_UP;
	}
	else if (_enemyAngle >= 3 * PI / 4 && _enemyAngle <= 5 * PI / 4)
	{
		_checkDirection = ENEMY_LEFT;
	}
	else if (_enemyAngle >= 5 * PI / 4 && _enemyAngle <= 7 * PI / 4)
	{
		_checkDirection = ENEMY_DOWN;
	}
	else if (_enemyAngle >= 7 * PI / 4 && _enemyAngle <= PI2 || _enemyAngle >= 0 && _enemyAngle <= PI / 4)
	{
		_checkDirection = ENEMY_RIGHT;
	}
}

void malmijal::detection()
{

}

void malmijal::stop()
{
	if (GetTickCount() - stopTime > 100)
	{
		stopTime = GetTickCount();
		_isMove = !_isMove;
	}
}