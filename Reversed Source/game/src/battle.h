#ifndef MCNCC_GAME_BATTLE_H
#define MCNCC_GAME_BATTLE_H

class CHARACTER;
bool timed_event_cancel(CHARACTER* character);
int battle_melee_attack(CHARACTER* attacker, CHARACTER* victim);
int battle_hit(CHARACTER* attacker, CHARACTER* victim, int* damage);
bool battle_distance_valid_by_xy(long x, long y, long targetX, long targetY);
bool battle_distance_valid(CHARACTER* character, CHARACTER* victim);
bool battle_is_attackable(CHARACTER* attacker, CHARACTER* victim);
void battle_end(CHARACTER* ch);
void battle_end_ex(CHARACTER* ch);
void NormalAttackAffect(CHARACTER* attacker, CHARACTER* victim);
unsigned long GET_ATTACK_SPEED(CHARACTER* character);
void SET_ATTACK_TIME(CHARACTER* attacker, CHARACTER* victim, unsigned long now);
void SET_ATTACKED_TIME(CHARACTER* attacker, CHARACTER* victim, unsigned long now);
bool IS_SPEED_HACK(CHARACTER* attacker, CHARACTER* victim, unsigned long now);
void SkillAttackAffect(CHARACTER* victim, int percent, unsigned long immunity, unsigned long type,
                       unsigned char point, long amount, unsigned long flag, int duration, const char* name);
class CItem;
long double CalcAttackRating(CHARACTER* attacker, CHARACTER* victim, bool ignoreTargetRating = false);
int CalcAttBonus(CHARACTER* attacker, CHARACTER* victim, int attack);
void Item_GetDamage(CItem* item, int* minimum, int* maximum);
int CalcBattleDamage(int damage, int attackerLevel, int victimLevel);
int CalcArrowDamage(CHARACTER* attacker, CHARACTER* victim, CItem* bow, CItem* arrow, bool ignoreDefense = false);
int CalcMeleeDamage(CHARACTER* attacker, CHARACTER* victim, bool ignoreDefense = false, bool ignoreTargetRating = false);
int CalcMagicDamageWithValue(int damage, CHARACTER* attacker, CHARACTER* victim);
int CalcMagicDamage(CHARACTER* attacker, CHARACTER* victim);

#endif
