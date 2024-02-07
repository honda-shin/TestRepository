#include "Puzzle1.h"
#include "3dobject/FontRenderer.h"
#include "3dobject/Sprite.h"
#include "Input.h"
#include "Light.h"

namespace Hoshiyama
{
	const int Puzzle1Script::add[][2] = {
		{ 0, -1 },
		{ 1, -1 },
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 },
		{ -1, 1 },
		{ -1, 0 },
		{ -1, -1 },
	};

	// ������
	void Puzzle1Script::Start()
	{
		// ���C�g�̐ݒ�
		Light* lit = Light::Create();

		InitCamera();

		// �t�H���g�L�����p�X�̐ݒ�
		m_FontCanvas = new FontCanvas(VIEWPORT_DISPLAY0_WIDTH / 13, VIEWPORT_DISPLAY0_HEIGHT / 13);
		m_FontCanvas->transform().localPosition = Vector3(0, 0, 1);
		m_FontCanvas->setLayer("UI_UP");
		m_FontCanvas->SetColor(1, 1, 1, 1);

		// �匳��GameObject���쐬
		m_Base = new GameObject();
		m_Base->setName("TitleBase");
		Transform* parentObj = &m_Base->transform();

		m_CellNum = 8;
		m_Cell = NULL;
		m_CellStatus = NULL;
		m_PlayerCellNum[0] = 0;
		m_PlayerCellNum[1] = 0;
		m_Turn = 0;

		m_Step = 1;
	}

	// ��Ꮘ��
	void Puzzle1Script::Update()
	{
		switch (m_Step)
		{
		case 1:
			Sprite* sprt;
			// �}�X�̕\��
			for (int i = 0; i <= m_CellNum; i++)
			{
				sprt = new Sprite();
				sprt->LoadTexture(NULL);
				sprt->SetTextureRect(0, 0, 24 * m_CellNum + 4, 4);
				sprt->setLayer("UI_DOWN");
				sprt->transform().SetParent(&m_Base->transform());
				sprt->transform().localPosition = Vector3(0, (-12 * m_CellNum) + i * 24, 0);

				sprt = new Sprite();
				sprt->LoadTexture(NULL);
				sprt->SetTextureRect(0, 0, 4, 24 * m_CellNum + 4);
				sprt->setLayer("UI_DOWN");
				sprt->transform().SetParent(&m_Base->transform());
				sprt->transform().localPosition = Vector3((-12 * m_CellNum) + i * 24, 0, 0);

			}

			// �R�}�̐ݒ�
			m_Cell = new Sprite*[m_CellNum * m_CellNum];
			m_CellStatus = new int[m_CellNum * m_CellNum];
			for (int i = 0; i < m_CellNum * m_CellNum; i++)
			{
				m_Cell[i] = new Sprite();
				m_Cell[i]->LoadTexture(NULL);
				m_Cell[i]->SetTextureRect(0, 0, 16, 16);
				m_Cell[i]->setLayer("UI_DOWN");
				m_Cell[i]->transform().SetParent(&m_Base->transform());
				m_Cell[i]->transform().localPosition = Vector3((-12 * m_CellNum) + (i % m_CellNum) * 24 + 12, 
															   (12 * m_CellNum) - (i / m_CellNum) * 24 - 12,
															   0);
				m_Cell[i]->SetActive(false);
				m_CellStatus[i] = 0;
			}

			m_FontCanvas->Print(0, 0, "�p�Y��");
			m_FontCanvas->Print(0, 2, "8x8�}�X�̒���8�̐Ԕ���u���Ă��������B\n���������݂����㉺���E�΂߂̈ʒu�łԂ���Ȃ��悤\n�z�u���Ă��������B");
			m_FontCanvas->Print(0, 6, "�^�b�`�@�@�@�z�u�E����\n�`�{�^���@�@����");

			m_SetNum = 0;
			m_TouchDown = false;
			m_Step = 2;
			break;

		case 2:
			if (Input::touchCount() != 0)
			{
				if (!m_TouchDown)
				{
					Vector2 touchPos = Input::GetTouch(0);

					int px = touchPos.x + -nn::gx::DISPLAY1_WIDTH / 2 + (12 * m_CellNum);
					int py = touchPos.y + -nn::gx::DISPLAY1_HEIGHT / 2 + (12 * m_CellNum);
					px /= 24;
					py /= 24;
					if (px >= 0 && px < m_CellNum && py >= 0 && py < m_CellNum)
					{
						int index = px + py * m_CellNum;
						bool check = m_CellStatus[index] == 0;
						if (check)
						{
							if (m_SetNum < 8)
							{
								SetStatus(index, 1);
								m_SetNum++;
							}
						}
						else
						{
							SetStatus(index, 0);
							m_SetNum--;
						}
					}
				}
				m_TouchDown = true;
			}
			else
			{
				m_TouchDown = false;
			}
			if (Input::GetButtonDown("ButtonA"))
			{
				DispStatus(Check()? 1 : 0);
			}
			break;
		}
	}

	// �e�L�X�g�\��
	void Puzzle1Script::DispStatus(int result)
	{
		m_FontCanvas->Clear(0, 17, 23, 1);
		if (result != 0)
		{
			m_FontCanvas->Print(1, 16, "�����I");
		}
		else
		{
			m_FontCanvas->Print(1, 16, "�Ⴄ��");
		}
	}

	// �}�X���̎擾
	int	Puzzle1Script::GetMasuStatus(int x, int y)  const
	{
		return (0 <= x && x < m_CellNum &&
				0 <= y && y < m_CellNum) ? m_CellStatus[y * m_CellNum + x] : -1;
	}

	// �R�}��u������
	void Puzzle1Script::SetStatus(int index, int value)
	{
		m_CellStatus[index] = value;
		if (value == 0)
		{
			m_Cell[index]->SetActive(false);
		}
		else
		{
			m_Cell[index]->SetActive(true);
			if (value == 1)
			{
				m_Cell[index]->SetColor(1, 0, 0, 1);
			}
			else
			{
				m_Cell[index]->SetColor(0, 0, 1, 1);
			}
		}
	}

	// �`�F�b�N
	bool Puzzle1Script::Check()
	{
		int count = 0;
		for (int i = 0; i < m_CellNum * m_CellNum; i++)
		{
			if (m_CellStatus[i] != 0)
			{
				if (Check_Sub(i))
				{
					return false;
				}
				count++;
			}
		}

		return count == 8;
	}

	// ������ɓ����l���������邩�`�F�b�N
	bool Puzzle1Script::Check_Sub(int index)
	{
		int bpx = index % m_CellNum;
		int bpy = index / m_CellNum;
		for (int i = 0; i < 8; i++)
		{
			int tpx = bpx + add[i][0];
			int tpy = bpy + add[i][1];
			int status;
			while ((status = GetMasuStatus(tpx, tpy)) >= 0)
			{
				if (status != 0)
				{
					// �Ԃ�����
					return true;
				}
				tpx += add[i][0];
				tpy += add[i][1];
			}
		}

		return false;
	}

	// �J�����̐ݒ�
	void Puzzle1Script::InitCamera()
	{
		Camera* cam;

		// �J�����̐ݒ�(����UI�p)
		cam = Camera::Create();
		cam->SetOrtho(-nn::gx::DISPLAY0_WIDTH / 2, nn::gx::DISPLAY0_WIDTH / 2,
						-nn::gx::DISPLAY0_HEIGHT / 2, nn::gx::DISPLAY0_HEIGHT / 2,
						0.1f, 100);
		cam->transform().localPosition = Vector3(0, 0, 10);
		cam->SetCameraTarget(0, 0, 0);
		cam->SetRenderTarget(Display0);
		cam->cullingMask = ~(1 << 5);
		cam->clearFlags = SolidColor;
		cam->backgroundColor = Vector3(0, 0, 0.2f);
		cam->gameObject().setName("CameraUpUI");

		// �J�����̐ݒ�(�����UI�p)
		cam = Camera::Create();
		cam->SetOrtho(-nn::gx::DISPLAY1_WIDTH / 2, nn::gx::DISPLAY1_WIDTH / 2,
						-nn::gx::DISPLAY1_HEIGHT / 2, nn::gx::DISPLAY1_HEIGHT / 2,
						0.1f, 100);
		cam->transform().localPosition = Vector3(0, 0, 10);
		cam->SetCameraTarget(0, 0, 0);
		cam->SetRenderTarget(Display1);
		cam->cullingMask = ~(1 << 6);
		cam->clearFlags = SolidColor;
		cam->backgroundColor = Vector3(0, 0, 0.2f);
		cam->gameObject().setName("CameraDownUI");
	}
}