#pragma once
#include "GameObject.h"
#include "MonoBehaviour.h"
#include "Vector2.h"

class FontCanvas;
class Sprite;

namespace Hoshiyama
{
	// Puzzle1スクリプト
	class Puzzle1Script : public MonoBehaviour
	{
	public:
		Puzzle1Script() {}
		virtual ~Puzzle1Script() {}

		void Start();
		void Update();

	private:
		static const int	add[][2];

		GameObject*			m_Base;
		FontCanvas*			m_FontCanvas;
		Sprite**			m_Cell;
		int*				m_CellStatus;
		int					m_CellNum;
		int					m_SetNum;
		bool				m_TouchDown;
		int					m_Turn;
		int					m_Step;
		int					m_PlayerCellNum[2];

		void DispStatus(int result);
		int GetMasuStatus(int x, int y)  const;
		void SetStatus(int index, int value);
		bool Check();
		bool Check_Sub(int index);
		void InitCamera();
	};

	// Othelloゲームオブジェクト
	class Puzzle1 : public GameObject
	{
	public:
		Puzzle1()
		{
			AddComponent(new Puzzle1Script());
		}
		virtual ~Puzzle1() {}
	};
}