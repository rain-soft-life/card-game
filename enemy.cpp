int Mdun = 0;//怪物护盾值
int Msp = 3;//怪物的魔力值

//怪物行为

//普通攻击
void Matk() {
	int hurt = 6;
	PBlood -= hurt;
	printf("怪物发起了攻击，对你造成了6点伤害。\n");
}
//防御
void Mdef() {
	//获得六点护盾

	Msp -= 2;
	if (Msp >= 0)
	{
		Mdun += 19;
		printf("怪物获得了9层护盾！\n");
	}
}
//恢复魔力
void Mregain() {
	Msp += 3;
	printf("怪物使用了魔力再生，恢复了3点魔力\n");
}
//撕咬
void bite() {
	Msp -= 4;
	int hurt = 18;
	PBlood -= hurt;
	printf("怪物使用了撕咬，对你造成了18点伤害\n");
}


//决策树
void DecideAction()
{
	int needDEF = (MBlood + Mdun < 7) || (Mdun == 0 && MBlood <= 20);
	if (R == 1) { Matk(); return; }
	if (Msp <= 1) { Mregain(); return; }
	if (needDEF && Msp >= 2) { Mdef(); return; }
	if (Msp >= 4 && PBlood <= 18) { bite(); return; }
	if (PBlood > 18 && Msp < 4) { Mregain(); return; }
	if (Msp >= 0) { Matk(); return; }
}
