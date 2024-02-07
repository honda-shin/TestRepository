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

	// 初期化
	void Puzzle1Script::Start()
	{
		// ライトの設定
		Light* lit = Light::Create();

		InitCamera();

		// フォントキャンパスの設定
		m_FontCanvas = new FontCanvas(VIEWPORT_DISPLAY0_WIDTH / 13, VIEWPORT_DISPLAY0_HEIGHT / 13);
		m_FontCanvas->transform().localPosition = Vector3(0, 0, 1);
		m_FontCanvas->setLayer("UI_UP");
		m_FontCanvas->SetColor(1, 1, 1, 1);

		// 大元のGameObjectを作成
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

	// 定例処理
	void Puzzle1Script::Update()
	{
		switch (m_Step)
		{
		case 1:
			Sprite* sprt;
			// マスの表示
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

			// コマの設定
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

			m_FontCanvas->Print(0, 0, "パズル");
			m_FontCanvas->Print(0, 2, "8x8マスの中に8個の赤箱を置いてください。\nただしお互いが上下左右斜めの位置でぶつからないよう\n配置してください。");
			m_FontCanvas->Print(0, 6, "タッチ　　　配置・解除\nＡボタン　　決定");

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

	// テキスト表示
	void Puzzle1Script::DispStatus(int result)
	{
		m_FontCanvas->Clear(0, 17, 23, 1);
		if (result != 0)
		{
			m_FontCanvas->Print(1, 16, "正解！");
		}
		else
		{
			m_FontCanvas->Print(1, 16, "違うよ");
		}
	}

	// マス情報の取得
	int	Puzzle1Script::GetMasuStatus(int x, int y)  const
	{
		return (0 <= x && x < m_CellNum &&
				0 <= y && y < m_CellNum) ? m_CellStatus[y * m_CellNum + x] : -1;
	}

	// コマを置く処理
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

	// チェック
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

	// 直線状に同じ値がいくつあるかチェック
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
					// ぶつかった
					return true;
				}
				tpx += add[i][0];
				tpy += add[i][1];
			}
		}

		return false;
	}

	// カメラの設定
	void Puzzle1Script::InitCamera()
	{
		Camera* cam;

		// カメラの設定(上画面UI用)
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

		// カメラの設定(下画面UI用)
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