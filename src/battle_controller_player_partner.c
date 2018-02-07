#include "global.h"
#include "battle.h"
#include "battle_controllers.h"
#include "battle_message.h"
#include "battle_interface.h"
#include "battle_anim.h"
#include "constants/battle_anim.h"
#include "battle_ai_script_commands.h"
#include "pokemon.h"
#include "link.h"
#include "util.h"
#include "main.h"
#include "constants/songs.h"
#include "sound.h"
#include "window.h"
#include "m4a.h"
#include "palette.h"
#include "task.h"
#include "text.h"
#include "string_util.h"
#include "bg.h"
#include "reshow_battle_screen.h"
#include "pokeball.h"
#include "data2.h"

extern u32 gBattleControllerExecFlags;
extern u8 gActiveBattler;
extern u8 gBattlerSpriteIds[MAX_BATTLERS_COUNT];
extern u8 gActionSelectionCursor[MAX_BATTLERS_COUNT];
extern u8 gBattlersCount;
extern bool8 gDoingBattleAnim;
extern void (*gBattlerControllerFuncs[MAX_BATTLERS_COUNT])(void);
extern void (*gPreBattleCallback1)(void);
extern u16 gBattlerPartyIndexes[MAX_BATTLERS_COUNT];
extern u8 gBattleBufferA[MAX_BATTLERS_COUNT][0x200];
extern u8 gBattleBufferB[MAX_BATTLERS_COUNT][0x200];
extern struct BattlePokemon gBattleMons[MAX_BATTLERS_COUNT];
extern struct SpriteTemplate gUnknown_0202499C;
extern u16 gSpecialVar_ItemId;
extern u8 gHealthboxSpriteIds[MAX_BATTLERS_COUNT];
extern u8 gBattleOutcome;
extern u16 gBattle_BG0_X;
extern u16 gBattle_BG0_Y;
extern u8 gBattlerInMenuId;
extern u16 gUnknown_020243FC;
extern u8 gUnknown_03005D7C[MAX_BATTLERS_COUNT];
extern u8 gBattleMonForms[MAX_BATTLERS_COUNT];
extern u16 gPartnerTrainerId;
extern u8 GetFrontierTrainerFrontSpriteId(u16 trainerId);
extern u8 gBattlerTarget;
extern u8 gAbsentBattlerFlags;
extern u8 gUnknown_020244B4[];
extern u32 gTransformedPersonalities[MAX_BATTLERS_COUNT];
extern struct UnusedControllerStruct gUnknown_02022D0C;

extern const struct CompressedSpritePalette gTrainerFrontPicPaletteTable[];
extern const struct CompressedSpritePalette gTrainerBackPicPaletteTable[];
extern const struct BattleMove gBattleMoves[];

extern void sub_81358F4(void);
extern void sub_8172EF0(u8 bank, struct Pokemon *mon);
extern void sub_806A068(u16, u8);

// this file's functions
static void PlayerPartnerHandleGetMonData(void);
static void PlayerPartnerHandleGetRawMonData(void);
static void PlayerPartnerHandleSetMonData(void);
static void PlayerPartnerHandleSetRawMonData(void);
static void PlayerPartnerHandleLoadMonSprite(void);
static void PlayerPartnerHandleSwitchInAnim(void);
static void PlayerPartnerHandleReturnMonToBall(void);
static void PlayerPartnerHandleDrawTrainerPic(void);
static void PlayerPartnerHandleTrainerSlide(void);
static void PlayerPartnerHandleTrainerSlideBack(void);
static void PlayerPartnerHandleFaintAnimation(void);
static void PlayerPartnerHandlePaletteFade(void);
static void PlayerPartnerHandleSuccessBallThrowAnim(void);
static void PlayerPartnerHandleBallThrowAnim(void);
static void PlayerPartnerHandlePause(void);
static void PlayerPartnerHandleMoveAnimation(void);
static void PlayerPartnerHandlePrintString(void);
static void PlayerPartnerHandlePrintSelectionString(void);
static void PlayerPartnerHandleChooseAction(void);
static void PlayerPartnerHandleUnknownYesNoBox(void);
static void PlayerPartnerHandleChooseMove(void);
static void PlayerPartnerHandleChooseItem(void);
static void PlayerPartnerHandleChoosePokemon(void);
static void PlayerPartnerHandleCmd23(void);
static void PlayerPartnerHandleHealthBarUpdate(void);
static void PlayerPartnerHandleExpUpdate(void);
static void PlayerPartnerHandleStatusIconUpdate(void);
static void PlayerPartnerHandleStatusAnimation(void);
static void PlayerPartnerHandleStatusXor(void);
static void PlayerPartnerHandleDataTransfer(void);
static void PlayerPartnerHandleDMA3Transfer(void);
static void PlayerPartnerHandlePlayBGM(void);
static void PlayerPartnerHandleCmd32(void);
static void PlayerPartnerHandleTwoReturnValues(void);
static void PlayerPartnerHandleChosenMonReturnValue(void);
static void PlayerPartnerHandleOneReturnValue(void);
static void PlayerPartnerHandleOneReturnValue_Duplicate(void);
static void PlayerPartnerHandleCmd37(void);
static void PlayerPartnerHandleCmd38(void);
static void PlayerPartnerHandleCmd39(void);
static void PlayerPartnerHandleCmd40(void);
static void PlayerPartnerHandleHitAnimation(void);
static void PlayerPartnerHandleCmd42(void);
static void PlayerPartnerHandlePlaySE(void);
static void PlayerPartnerHandlePlayFanfareOrBGM(void);
static void PlayerPartnerHandleFaintingCry(void);
static void PlayerPartnerHandleIntroSlide(void);
static void PlayerPartnerHandleIntroTrainerBallThrow(void);
static void PlayerPartnerHandleDrawPartyStatusSummary(void);
static void PlayerPartnerHandleCmd49(void);
static void PlayerPartnerHandleCmd50(void);
static void PlayerPartnerHandleSpriteInvisibility(void);
static void PlayerPartnerHandleBattleAnimation(void);
static void PlayerPartnerHandleLinkStandbyMsg(void);
static void PlayerPartnerHandleResetActionMoveSelection(void);
static void PlayerPartnerHandleCmd55(void);
static void nullsub_128(void);

static void PlayerPartnerBufferRunCommand(void);
static void PlayerPartnerBufferExecCompleted(void);
static void sub_81BB628(u8 taskId);
static void DestroyExpTaskAndCompleteOnInactiveTextPrinter(u8 taskId);
static void Task_PrepareToGiveExpWithExpBar(u8 taskId);
static void sub_81BB4E4(u8 taskId);
static void sub_81BB628(u8 taskId);
static void sub_81BB688(u8 taskId);
static void sub_81BB9A0(void);
static u32 CopyPlayerPartnerMonData(u8 monId, u8 *dst);
static void SetPlayerPartnerMonData(u8 monId);
static void sub_81BD0E4(u8 bank, bool8 dontClearSubstituteBit);
static void DoSwitchOutAnimation(void);
static void PlayerPartnerDoMoveAnimation(void);
static void sub_81BE2C8(u8 taskId);
static void sub_81BE498(void);

static void (*const sPlayerPartnerBufferCommands[CONTROLLER_CMDS_COUNT])(void) =
{
    PlayerPartnerHandleGetMonData,
    PlayerPartnerHandleGetRawMonData,
    PlayerPartnerHandleSetMonData,
    PlayerPartnerHandleSetRawMonData,
    PlayerPartnerHandleLoadMonSprite,
    PlayerPartnerHandleSwitchInAnim,
    PlayerPartnerHandleReturnMonToBall,
    PlayerPartnerHandleDrawTrainerPic,
    PlayerPartnerHandleTrainerSlide,
    PlayerPartnerHandleTrainerSlideBack,
    PlayerPartnerHandleFaintAnimation,
    PlayerPartnerHandlePaletteFade,
    PlayerPartnerHandleSuccessBallThrowAnim,
    PlayerPartnerHandleBallThrowAnim,
    PlayerPartnerHandlePause,
    PlayerPartnerHandleMoveAnimation,
    PlayerPartnerHandlePrintString,
    PlayerPartnerHandlePrintSelectionString,
    PlayerPartnerHandleChooseAction,
    PlayerPartnerHandleUnknownYesNoBox,
    PlayerPartnerHandleChooseMove,
    PlayerPartnerHandleChooseItem,
    PlayerPartnerHandleChoosePokemon,
    PlayerPartnerHandleCmd23,
    PlayerPartnerHandleHealthBarUpdate,
    PlayerPartnerHandleExpUpdate,
    PlayerPartnerHandleStatusIconUpdate,
    PlayerPartnerHandleStatusAnimation,
    PlayerPartnerHandleStatusXor,
    PlayerPartnerHandleDataTransfer,
    PlayerPartnerHandleDMA3Transfer,
    PlayerPartnerHandlePlayBGM,
    PlayerPartnerHandleCmd32,
    PlayerPartnerHandleTwoReturnValues,
    PlayerPartnerHandleChosenMonReturnValue,
    PlayerPartnerHandleOneReturnValue,
    PlayerPartnerHandleOneReturnValue_Duplicate,
    PlayerPartnerHandleCmd37,
    PlayerPartnerHandleCmd38,
    PlayerPartnerHandleCmd39,
    PlayerPartnerHandleCmd40,
    PlayerPartnerHandleHitAnimation,
    PlayerPartnerHandleCmd42,
    PlayerPartnerHandlePlaySE,
    PlayerPartnerHandlePlayFanfareOrBGM,
    PlayerPartnerHandleFaintingCry,
    PlayerPartnerHandleIntroSlide,
    PlayerPartnerHandleIntroTrainerBallThrow,
    PlayerPartnerHandleDrawPartyStatusSummary,
    PlayerPartnerHandleCmd49,
    PlayerPartnerHandleCmd50,
    PlayerPartnerHandleSpriteInvisibility,
    PlayerPartnerHandleBattleAnimation,
    PlayerPartnerHandleLinkStandbyMsg,
    PlayerPartnerHandleResetActionMoveSelection,
    PlayerPartnerHandleCmd55,
    nullsub_128
};

// unknown unused data
static const u8 sUnknown_08617254[] =
{
    0x83, 0x4d, 0xf3, 0x5f, 0x6f, 0x4f, 0xeb, 0x3e,
    0x67, 0x2e, 0x10, 0x46, 0x8c, 0x3d, 0x28, 0x35,
    0xc5, 0x2c, 0x15, 0x7f, 0xb5, 0x56, 0x9d, 0x53,
    0x3b, 0x43, 0xda, 0x36, 0x79, 0x2a, 0x0e, 0x53,
};

static void nullsub_77(void)
{
}

void SetControllerToPlayerPartner(void)
{
    gBattlerControllerFuncs[gActiveBattler] = PlayerPartnerBufferRunCommand;
}

static void PlayerPartnerBufferRunCommand(void)
{
    if (gBattleControllerExecFlags & gBitTable[gActiveBattler])
    {
        if (gBattleBufferA[gActiveBattler][0] < ARRAY_COUNT(sPlayerPartnerBufferCommands))
            sPlayerPartnerBufferCommands[gBattleBufferA[gActiveBattler][0]]();
        else
            PlayerPartnerBufferExecCompleted();
    }
}

static void CompleteOnBankSpriteCallbackDummy(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
        PlayerPartnerBufferExecCompleted();
}

static void sub_81BAE98(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
    {
        nullsub_25(0);
        FreeSpriteOamMatrix(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        DestroySprite(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        PlayerPartnerBufferExecCompleted();
    }
}

static void sub_81BAF00(void)
{
    if (--gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].field_9 == 0xFF)
    {
        gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].field_9 = 0;
        PlayerPartnerBufferExecCompleted();
    }
}

static void sub_81BAF48(void)
{
    bool32 r6 = FALSE;

    if (!IsDoubleBattle() || (IsDoubleBattle() && (gBattleTypeFlags & BATTLE_TYPE_MULTI)))
    {
        if (gSprites[gHealthboxSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
            r6 = TRUE;
    }
    else
    {
        if (gSprites[gHealthboxSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy
            && gSprites[gHealthboxSpriteIds[gActiveBattler ^ BIT_FLANK]].callback == SpriteCallbackDummy)
        {
            r6 = TRUE;
        }
    }

    if (IsCryPlayingOrClearCrySongs())
        r6 = FALSE;

    if (r6)
    {
        gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].field_9 = 3;
        gBattlerControllerFuncs[gActiveBattler] = sub_81BAF00;
    }
}

static void sub_81BB02C(void)
{
    if (!gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].ballAnimActive
        && !gBattleSpritesDataPtr->healthBoxesData[gActiveBattler ^ BIT_FLANK].ballAnimActive
        && gSprites[gUnknown_03005D7C[gActiveBattler]].callback == SpriteCallbackDummy
        && gSprites[gBattlerSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy
        && ++gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].field_9 != 1)
    {
        gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].field_9 = 0;

        if (IsDoubleBattle() && !(gBattleTypeFlags & BATTLE_TYPE_MULTI))
        {
            DestroySprite(&gSprites[gUnknown_03005D7C[gActiveBattler ^ BIT_FLANK]]);
            UpdateHealthboxAttribute(gHealthboxSpriteIds[gActiveBattler ^ BIT_FLANK], &gPlayerParty[gBattlerPartyIndexes[gActiveBattler ^ BIT_FLANK]], HEALTHBOX_ALL);
            sub_8076918(gActiveBattler ^ BIT_FLANK);
            SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler ^ BIT_FLANK]);
        }

        DestroySprite(&gSprites[gUnknown_03005D7C[gActiveBattler]]);
        UpdateHealthboxAttribute(gHealthboxSpriteIds[gActiveBattler], &gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], HEALTHBOX_ALL);
        sub_8076918(gActiveBattler);
        SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler]);

        gBattleSpritesDataPtr->animationData->field_9_x1 = 0;

        gBattlerControllerFuncs[gActiveBattler] = sub_81BAF48;
    }
}

static void sub_81BB1D4(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].animEnded && gSprites[gBattlerSpriteIds[gActiveBattler]].pos2.x == 0)
        PlayerPartnerBufferExecCompleted();
}

static void CompleteOnHealthbarDone(void)
{
    s16 hpValue = sub_8074AA0(gActiveBattler, gHealthboxSpriteIds[gActiveBattler], HEALTH_BAR, 0);

    SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler]);

    if (hpValue != -1)
    {
        UpdateHpTextInHealthbox(gHealthboxSpriteIds[gActiveBattler], hpValue, HP_CURRENT);
    }
    else
    {
        HandleLowHpMusicChange(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
        PlayerPartnerBufferExecCompleted();
    }
}

static void CompleteOnInactiveTextPrinter(void)
{
    if (!IsTextPrinterActive(0))
        PlayerPartnerBufferExecCompleted();
}

// the whole exp task is copied&pasted from player controller
#define tExpTask_monId      data[0]
#define tExpTask_gainedExp  data[1]
#define tExpTask_bank       data[2]
#define tExpTask_frames     data[10]

static void Task_GiveExpToMon(u8 taskId)
{
    u32 monId = (u8)(gTasks[taskId].tExpTask_monId);
    u8 bank = gTasks[taskId].tExpTask_bank;
    s16 gainedExp = gTasks[taskId].tExpTask_gainedExp;

    if (IsDoubleBattle() == TRUE || monId != gBattlerPartyIndexes[bank]) // give exp without the expbar
    {
        struct Pokemon *mon = &gPlayerParty[monId];
        u16 species = GetMonData(mon, MON_DATA_SPECIES);
        u8 level = GetMonData(mon, MON_DATA_LEVEL);
        u32 currExp = GetMonData(mon, MON_DATA_EXP);
        u32 nextLvlExp = gExperienceTables[gBaseStats[species].growthRate][level + 1];

        if (currExp + gainedExp >= nextLvlExp)
        {
            u8 savedActiveBank;

            SetMonData(mon, MON_DATA_EXP, &nextLvlExp);
            CalculateMonStats(mon);
            gainedExp -= nextLvlExp - currExp;
            savedActiveBank = gActiveBattler;
            gActiveBattler = bank;
            BtlController_EmitTwoReturnValues(1, RET_VALUE_LEVELLED_UP, gainedExp);
            gActiveBattler = savedActiveBank;

            if (IsDoubleBattle() == TRUE
             && ((u16)(monId) == gBattlerPartyIndexes[bank] || (u16)(monId) == gBattlerPartyIndexes[bank ^ BIT_FLANK]))
                gTasks[taskId].func = sub_81BB628;
            else
                gTasks[taskId].func = DestroyExpTaskAndCompleteOnInactiveTextPrinter;
        }
        else
        {
            currExp += gainedExp;
            SetMonData(mon, MON_DATA_EXP, &currExp);
            gBattlerControllerFuncs[bank] = CompleteOnInactiveTextPrinter;
            DestroyTask(taskId);
        }
    }
    else
    {
        gTasks[taskId].func = Task_PrepareToGiveExpWithExpBar;
    }
}

static void Task_PrepareToGiveExpWithExpBar(u8 taskId)
{
    u8 monIndex = gTasks[taskId].tExpTask_monId;
    s32 gainedExp = gTasks[taskId].tExpTask_gainedExp;
    u8 bank = gTasks[taskId].tExpTask_bank;
    struct Pokemon *mon = &gPlayerParty[monIndex];
    u8 level = GetMonData(mon, MON_DATA_LEVEL);
    u16 species = GetMonData(mon, MON_DATA_SPECIES);
    u32 exp = GetMonData(mon, MON_DATA_EXP);
    u32 currLvlExp = gExperienceTables[gBaseStats[species].growthRate][level];
    u32 expToNextLvl;

    exp -= currLvlExp;
    expToNextLvl = gExperienceTables[gBaseStats[species].growthRate][level + 1] - currLvlExp;
    SetBattleBarStruct(bank, gHealthboxSpriteIds[bank], expToNextLvl, exp, -gainedExp);
    PlaySE(SE_EXP);
    gTasks[taskId].func = sub_81BB4E4;
}

static void sub_81BB4E4(u8 taskId)
{
    if (gTasks[taskId].tExpTask_frames < 13)
    {
        gTasks[taskId].tExpTask_frames++;
    }
    else
    {
        u8 monId = gTasks[taskId].tExpTask_monId;
        s16 gainedExp = gTasks[taskId].tExpTask_gainedExp;
        u8 bank = gTasks[taskId].tExpTask_bank;
        s16 r4;

        r4 = sub_8074AA0(bank, gHealthboxSpriteIds[bank], EXP_BAR, 0);
        SetHealthboxSpriteVisible(gHealthboxSpriteIds[bank]);
        if (r4 == -1)
        {
            u8 level;
            s32 currExp;
            u16 species;
            s32 expOnNextLvl;

            m4aSongNumStop(SE_EXP);
            level = GetMonData(&gPlayerParty[monId], MON_DATA_LEVEL);
            currExp = GetMonData(&gPlayerParty[monId], MON_DATA_EXP);
            species = GetMonData(&gPlayerParty[monId], MON_DATA_SPECIES);
            expOnNextLvl = gExperienceTables[gBaseStats[species].growthRate][level + 1];

            if (currExp + gainedExp >= expOnNextLvl)
            {
                u8 savedActiveBank;

                SetMonData(&gPlayerParty[monId], MON_DATA_EXP, &expOnNextLvl);
                CalculateMonStats(&gPlayerParty[monId]);
                gainedExp -= expOnNextLvl - currExp;
                savedActiveBank = gActiveBattler;
                gActiveBattler = bank;
                BtlController_EmitTwoReturnValues(1, RET_VALUE_LEVELLED_UP, gainedExp);
                gActiveBattler = savedActiveBank;
                gTasks[taskId].func = sub_81BB628;
            }
            else
            {
                currExp += gainedExp;
                SetMonData(&gPlayerParty[monId], MON_DATA_EXP, &currExp);
                gBattlerControllerFuncs[bank] = CompleteOnInactiveTextPrinter;
                DestroyTask(taskId);
            }
        }
    }
}

static void sub_81BB628(u8 taskId)
{
    u8 bank = gTasks[taskId].tExpTask_bank;
    u8 monIndex = gTasks[taskId].tExpTask_monId;

    if (IsDoubleBattle() == TRUE && monIndex == gBattlerPartyIndexes[bank ^ BIT_FLANK])
        bank ^= BIT_FLANK;

    InitAndLaunchSpecialAnimation(bank, bank, bank, B_ANIM_LVL_UP);
    gTasks[taskId].func = sub_81BB688;
}

static void sub_81BB688(u8 taskId)
{
    u8 bank = gTasks[taskId].tExpTask_bank;

    if (!gBattleSpritesDataPtr->healthBoxesData[bank].specialAnimActive)
    {
        u8 monIndex = gTasks[taskId].tExpTask_monId;

        GetMonData(&gPlayerParty[monIndex], MON_DATA_LEVEL);  // Unused return value

        if (IsDoubleBattle() == TRUE && monIndex == gBattlerPartyIndexes[bank ^ BIT_FLANK])
            UpdateHealthboxAttribute(gHealthboxSpriteIds[bank ^ BIT_FLANK], &gPlayerParty[monIndex], HEALTHBOX_ALL);
        else
            UpdateHealthboxAttribute(gHealthboxSpriteIds[bank], &gPlayerParty[monIndex], HEALTHBOX_ALL);

        gTasks[taskId].func = DestroyExpTaskAndCompleteOnInactiveTextPrinter;
    }
}

static void DestroyExpTaskAndCompleteOnInactiveTextPrinter(u8 taskId)
{
    u8 monIndex;
    u8 bank;

    monIndex = gTasks[taskId].tExpTask_monId;
    GetMonData(&gPlayerParty[monIndex], MON_DATA_LEVEL);  // Unused return value
    bank = gTasks[taskId].tExpTask_bank;
    gBattlerControllerFuncs[bank] = CompleteOnInactiveTextPrinter;
    DestroyTask(taskId);
}

static void sub_81BB78C(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].pos1.y + gSprites[gBattlerSpriteIds[gActiveBattler]].pos2.y > DISPLAY_HEIGHT)
    {
        u16 species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES);

        nullsub_24(species);
        FreeOamMatrix(gSprites[gBattlerSpriteIds[gActiveBattler]].oam.matrixNum);
        DestroySprite(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        SetHealthboxSpriteInvisible(gHealthboxSpriteIds[gActiveBattler]);
        PlayerPartnerBufferExecCompleted();
    }
}

static void sub_81BB828(void)
{
    if (!gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].specialAnimActive)
    {
        FreeSpriteOamMatrix(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        DestroySprite(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        SetHealthboxSpriteInvisible(gHealthboxSpriteIds[gActiveBattler]);
        PlayerPartnerBufferExecCompleted();
    }
}

static void CompleteOnInactiveTextPrinter2(void)
{
    if (!IsTextPrinterActive(0))
        PlayerPartnerBufferExecCompleted();
}

static void DoHitAnimBlinkSpriteEffect(void)
{
    u8 spriteId = gBattlerSpriteIds[gActiveBattler];

    if (gSprites[spriteId].data[1] == 32)
    {
        gSprites[spriteId].data[1] = 0;
        gSprites[spriteId].invisible = 0;
        gDoingBattleAnim = FALSE;
        PlayerPartnerBufferExecCompleted();
    }
    else
    {
        if ((gSprites[spriteId].data[1] % 4) == 0)
            gSprites[spriteId].invisible ^= 1;
        gSprites[spriteId].data[1]++;
    }
}

static void sub_81BB92C(void)
{
    if (gSprites[gHealthboxSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
    {
        CopyBattleSpriteInvisibility(gActiveBattler);
        if (gBattleSpritesDataPtr->battlerData[gActiveBattler].behindSubstitute)
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, B_ANIM_MON_TO_SUBSTITUTE);

        gBattlerControllerFuncs[gActiveBattler] = sub_81BB9A0;
    }
}

static void sub_81BB9A0(void)
{
    if (!gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].specialAnimActive
        && gSprites[gBattlerSpriteIds[gActiveBattler]].callback == SpriteCallbackDummy)
    {
        PlayerPartnerBufferExecCompleted();
    }
}

static void sub_81BB9F4(void)
{
    if (gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].field_1_x1)
    {
        gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].flag_x80 = 0;
        gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].field_1_x1 = 0;

        FreeSpriteTilesByTag(0x27F9);
        FreeSpritePaletteByTag(0x27F9);

        CreateTask(c3_0802FDF4, 10);
        HandleLowHpMusicChange(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
        StartSpriteAnim(&gSprites[gBattlerSpriteIds[gActiveBattler]], 0);
        UpdateHealthboxAttribute(gHealthboxSpriteIds[gActiveBattler], &gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], HEALTHBOX_ALL);
        sub_8076918(gActiveBattler);
        SetHealthboxSpriteVisible(gHealthboxSpriteIds[gActiveBattler]);

        gBattlerControllerFuncs[gActiveBattler] = sub_81BB92C;
    }
}

static void sub_81BBAE8(void)
{
    if (!gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].flag_x80
        && !gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].ballAnimActive)
    {
        sub_8172EF0(gActiveBattler, &gPlayerParty[gBattlerPartyIndexes[gActiveBattler]]);
    }

    if (gSprites[gUnknown_03005D7C[gActiveBattler]].callback == SpriteCallbackDummy
     && !gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].ballAnimActive)
    {
        DestroySprite(&gSprites[gUnknown_03005D7C[gActiveBattler]]);
        gBattlerControllerFuncs[gActiveBattler] = sub_81BB9F4;
    }
}

static void PlayerPartnerBufferExecCompleted(void)
{
    gBattlerControllerFuncs[gActiveBattler] = PlayerPartnerBufferRunCommand;
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        u8 playerId = GetMultiplayerId();

        PrepareBufferDataTransferLink(2, 4, &playerId);
        gBattleBufferA[gActiveBattler][0] = CONTROLLER_TERMINATOR_NOP;
    }
    else
    {
        gBattleControllerExecFlags &= ~gBitTable[gActiveBattler];
    }
}

static void CompleteOnFinishedStatusAnimation(void)
{
    if (!gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].statusAnimActive)
        PlayerPartnerBufferExecCompleted();
}

static void CompleteOnFinishedBattleAnimation(void)
{
    if (!gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].animFromTableActive)
        PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleGetMonData(void)
{
    u8 monData[sizeof(struct Pokemon) * 2 + 56]; // this allows to get full data of two pokemon, trying to get more will result in overwriting data
    u32 size = 0;
    u8 monToCheck;
    s32 i;

    if (gBattleBufferA[gActiveBattler][2] == 0)
    {
        size += CopyPlayerPartnerMonData(gBattlerPartyIndexes[gActiveBattler], monData);
    }
    else
    {
        monToCheck = gBattleBufferA[gActiveBattler][2];
        for (i = 0; i < 6; i++)
        {
            if (monToCheck & 1)
                size += CopyPlayerPartnerMonData(i, monData + size);
            monToCheck >>= 1;
        }
    }
    BtlController_EmitDataTransfer(1, size, monData);
    PlayerPartnerBufferExecCompleted();
}

static u32 CopyPlayerPartnerMonData(u8 monId, u8 *dst)
{
    struct BattlePokemon battleMon;
    struct MovePpInfo moveData;
    u8 nickname[20];
    u8 *src;
    s16 data16;
    u32 data32;
    s32 size = 0;

    switch (gBattleBufferA[gActiveBattler][1])
    {
    case REQUEST_ALL_BATTLE:
        battleMon.species = GetMonData(&gPlayerParty[monId], MON_DATA_SPECIES);
        battleMon.item = GetMonData(&gPlayerParty[monId], MON_DATA_HELD_ITEM);
        for (size = 0; size < 4; size++)
        {
            battleMon.moves[size] = GetMonData(&gPlayerParty[monId], MON_DATA_MOVE1 + size);
            battleMon.pp[size] = GetMonData(&gPlayerParty[monId], MON_DATA_PP1 + size);
        }
        battleMon.ppBonuses = GetMonData(&gPlayerParty[monId], MON_DATA_PP_BONUSES);
        battleMon.friendship = GetMonData(&gPlayerParty[monId], MON_DATA_FRIENDSHIP);
        battleMon.experience = GetMonData(&gPlayerParty[monId], MON_DATA_EXP);
        battleMon.hpIV = GetMonData(&gPlayerParty[monId], MON_DATA_HP_IV);
        battleMon.attackIV = GetMonData(&gPlayerParty[monId], MON_DATA_ATK_IV);
        battleMon.defenseIV = GetMonData(&gPlayerParty[monId], MON_DATA_DEF_IV);
        battleMon.speedIV = GetMonData(&gPlayerParty[monId], MON_DATA_SPEED_IV);
        battleMon.spAttackIV = GetMonData(&gPlayerParty[monId], MON_DATA_SPATK_IV);
        battleMon.spDefenseIV = GetMonData(&gPlayerParty[monId], MON_DATA_SPDEF_IV);
        battleMon.personality = GetMonData(&gPlayerParty[monId], MON_DATA_PERSONALITY);
        battleMon.status1 = GetMonData(&gPlayerParty[monId], MON_DATA_STATUS);
        battleMon.level = GetMonData(&gPlayerParty[monId], MON_DATA_LEVEL);
        battleMon.hp = GetMonData(&gPlayerParty[monId], MON_DATA_HP);
        battleMon.maxHP = GetMonData(&gPlayerParty[monId], MON_DATA_MAX_HP);
        battleMon.attack = GetMonData(&gPlayerParty[monId], MON_DATA_ATK);
        battleMon.defense = GetMonData(&gPlayerParty[monId], MON_DATA_DEF);
        battleMon.speed = GetMonData(&gPlayerParty[monId], MON_DATA_SPEED);
        battleMon.spAttack = GetMonData(&gPlayerParty[monId], MON_DATA_SPATK);
        battleMon.spDefense = GetMonData(&gPlayerParty[monId], MON_DATA_SPDEF);
        battleMon.isEgg = GetMonData(&gPlayerParty[monId], MON_DATA_IS_EGG);
        battleMon.altAbility = GetMonData(&gPlayerParty[monId], MON_DATA_ALT_ABILITY);
        battleMon.otId = GetMonData(&gPlayerParty[monId], MON_DATA_OT_ID);
        GetMonData(&gPlayerParty[monId], MON_DATA_NICKNAME, nickname);
        StringCopy10(battleMon.nickname, nickname);
        GetMonData(&gPlayerParty[monId], MON_DATA_OT_NAME, battleMon.otName);
        src = (u8 *)&battleMon;
        for (size = 0; size < sizeof(battleMon); size++)
            dst[size] = src[size];
        break;
    case REQUEST_SPECIES_BATTLE:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_SPECIES);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case REQUEST_HELDITEM_BATTLE:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_HELD_ITEM);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case REQUEST_MOVES_PP_BATTLE:
        for (size = 0; size < 4; size++)
        {
            moveData.moves[size] = GetMonData(&gPlayerParty[monId], MON_DATA_MOVE1 + size);
            moveData.pp[size] = GetMonData(&gPlayerParty[monId], MON_DATA_PP1 + size);
        }
        moveData.ppBonuses = GetMonData(&gPlayerParty[monId], MON_DATA_PP_BONUSES);
        src = (u8*)(&moveData);
        for (size = 0; size < sizeof(moveData); size++)
            dst[size] = src[size];
        break;
    case REQUEST_MOVE1_BATTLE:
    case REQUEST_MOVE2_BATTLE:
    case REQUEST_MOVE3_BATTLE:
    case REQUEST_MOVE4_BATTLE:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_MOVE1 + gBattleBufferA[gActiveBattler][1] - REQUEST_MOVE1_BATTLE);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case REQUEST_PP_DATA_BATTLE:
        for (size = 0; size < 4; size++)
            dst[size] = GetMonData(&gPlayerParty[monId], MON_DATA_PP1 + size);
        dst[size] = GetMonData(&gPlayerParty[monId], MON_DATA_PP_BONUSES);
        size++;
        break;
    case REQUEST_PPMOVE1_BATTLE:
    case REQUEST_PPMOVE2_BATTLE:
    case REQUEST_PPMOVE3_BATTLE:
    case REQUEST_PPMOVE4_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_PP1 + gBattleBufferA[gActiveBattler][1] - REQUEST_PPMOVE1_BATTLE);
        size = 1;
        break;
    case REQUEST_OTID_BATTLE:
        data32 = GetMonData(&gPlayerParty[monId], MON_DATA_OT_ID);
        dst[0] = (data32 & 0x000000FF);
        dst[1] = (data32 & 0x0000FF00) >> 8;
        dst[2] = (data32 & 0x00FF0000) >> 16;
        size = 3;
        break;
    case REQUEST_EXP_BATTLE:
        data32 = GetMonData(&gPlayerParty[monId], MON_DATA_EXP);
        dst[0] = (data32 & 0x000000FF);
        dst[1] = (data32 & 0x0000FF00) >> 8;
        dst[2] = (data32 & 0x00FF0000) >> 16;
        size = 3;
        break;
    case REQUEST_HP_EV_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_HP_EV);
        size = 1;
        break;
    case REQUEST_ATK_EV_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_ATK_EV);
        size = 1;
        break;
    case REQUEST_DEF_EV_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_DEF_EV);
        size = 1;
        break;
    case REQUEST_SPEED_EV_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_SPEED_EV);
        size = 1;
        break;
    case REQUEST_SPATK_EV_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_SPATK_EV);
        size = 1;
        break;
    case REQUEST_SPDEF_EV_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_SPDEF_EV);
        size = 1;
        break;
    case REQUEST_FRIENDSHIP_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_FRIENDSHIP);
        size = 1;
        break;
    case REQUEST_POKERUS_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_POKERUS);
        size = 1;
        break;
    case REQUEST_MET_LOCATION_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_MET_LOCATION);
        size = 1;
        break;
    case REQUEST_MET_LEVEL_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_MET_LEVEL);
        size = 1;
        break;
    case REQUEST_MET_GAME_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_MET_GAME);
        size = 1;
        break;
    case REQUEST_POKEBALL_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_POKEBALL);
        size = 1;
        break;
    case REQUEST_ALL_IVS_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_HP_IV);
        dst[1] = GetMonData(&gPlayerParty[monId], MON_DATA_ATK_IV);
        dst[2] = GetMonData(&gPlayerParty[monId], MON_DATA_DEF_IV);
        dst[3] = GetMonData(&gPlayerParty[monId], MON_DATA_SPEED_IV);
        dst[4] = GetMonData(&gPlayerParty[monId], MON_DATA_SPATK_IV);
        dst[5] = GetMonData(&gPlayerParty[monId], MON_DATA_SPDEF_IV);
        size = 6;
        break;
    case REQUEST_HP_IV_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_HP_IV);
        size = 1;
        break;
    case REQUEST_ATK_IV_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_ATK_IV);
        size = 1;
        break;
    case REQUEST_DEF_IV_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_DEF_IV);
        size = 1;
        break;
    case REQUEST_SPEED_IV_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_SPEED_IV);
        size = 1;
        break;
    case REQUEST_SPATK_IV_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_SPATK_IV);
        size = 1;
        break;
    case REQUEST_SPDEF_IV_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_SPDEF_IV);
        size = 1;
        break;
    case REQUEST_PERSONALITY_BATTLE:
        data32 = GetMonData(&gPlayerParty[monId], MON_DATA_PERSONALITY);
        dst[0] = (data32 & 0x000000FF);
        dst[1] = (data32 & 0x0000FF00) >> 8;
        dst[2] = (data32 & 0x00FF0000) >> 16;
        dst[3] = (data32 & 0xFF000000) >> 24;
        size = 4;
        break;
    case REQUEST_CHECKSUM_BATTLE:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_CHECKSUM);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case REQUEST_STATUS_BATTLE:
        data32 = GetMonData(&gPlayerParty[monId], MON_DATA_STATUS);
        dst[0] = (data32 & 0x000000FF);
        dst[1] = (data32 & 0x0000FF00) >> 8;
        dst[2] = (data32 & 0x00FF0000) >> 16;
        dst[3] = (data32 & 0xFF000000) >> 24;
        size = 4;
        break;
    case REQUEST_LEVEL_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_LEVEL);
        size = 1;
        break;
    case REQUEST_HP_BATTLE:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_HP);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case REQUEST_MAX_HP_BATTLE:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_MAX_HP);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case REQUEST_ATK_BATTLE:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_ATK);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case REQUEST_DEF_BATTLE:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_DEF);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case REQUEST_SPEED_BATTLE:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_SPEED);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case REQUEST_SPATK_BATTLE:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_SPATK);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case REQUEST_SPDEF_BATTLE:
        data16 = GetMonData(&gPlayerParty[monId], MON_DATA_SPDEF);
        dst[0] = data16;
        dst[1] = data16 >> 8;
        size = 2;
        break;
    case REQUEST_COOL_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_COOL);
        size = 1;
        break;
    case REQUEST_BEAUTY_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_BEAUTY);
        size = 1;
        break;
    case REQUEST_CUTE_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_CUTE);
        size = 1;
        break;
    case REQUEST_SMART_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_SMART);
        size = 1;
        break;
    case REQUEST_TOUGH_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_TOUGH);
        size = 1;
        break;
    case REQUEST_SHEEN_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_SHEEN);
        size = 1;
        break;
    case REQUEST_COOL_RIBBON_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_COOL_RIBBON);
        size = 1;
        break;
    case REQUEST_BEAUTY_RIBBON_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_BEAUTY_RIBBON);
        size = 1;
        break;
    case REQUEST_CUTE_RIBBON_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_CUTE_RIBBON);
        size = 1;
        break;
    case REQUEST_SMART_RIBBON_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_SMART_RIBBON);
        size = 1;
        break;
    case REQUEST_TOUGH_RIBBON_BATTLE:
        dst[0] = GetMonData(&gPlayerParty[monId], MON_DATA_TOUGH_RIBBON);
        size = 1;
        break;
    }

    return size;
}

static void PlayerPartnerHandleGetRawMonData(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleSetMonData(void)
{
    u8 monToCheck;
    u8 i;

    if (gBattleBufferA[gActiveBattler][2] == 0)
    {
        SetPlayerPartnerMonData(gBattlerPartyIndexes[gActiveBattler]);
    }
    else
    {
        monToCheck = gBattleBufferA[gActiveBattler][2];
        for (i = 0; i < 6; i++)
        {
            if (monToCheck & 1)
                SetPlayerPartnerMonData(i);
            monToCheck >>= 1;
        }
    }
    PlayerPartnerBufferExecCompleted();
}

static void SetPlayerPartnerMonData(u8 monId)
{
    struct BattlePokemon *battlePokemon = (struct BattlePokemon *)&gBattleBufferA[gActiveBattler][3];
    struct MovePpInfo *moveData = (struct MovePpInfo *)&gBattleBufferA[gActiveBattler][3];
    s32 i;

    switch (gBattleBufferA[gActiveBattler][1])
    {
    case REQUEST_ALL_BATTLE:
        {
            u8 iv;

            SetMonData(&gPlayerParty[monId], MON_DATA_SPECIES, &battlePokemon->species);
            SetMonData(&gPlayerParty[monId], MON_DATA_HELD_ITEM, &battlePokemon->item);
            for (i = 0; i < 4; i++)
            {
                SetMonData(&gPlayerParty[monId], MON_DATA_MOVE1 + i, &battlePokemon->moves[i]);
                SetMonData(&gPlayerParty[monId], MON_DATA_PP1 + i, &battlePokemon->pp[i]);
            }
            SetMonData(&gPlayerParty[monId], MON_DATA_PP_BONUSES, &battlePokemon->ppBonuses);
            SetMonData(&gPlayerParty[monId], MON_DATA_FRIENDSHIP, &battlePokemon->friendship);
            SetMonData(&gPlayerParty[monId], MON_DATA_EXP, &battlePokemon->experience);
            iv = battlePokemon->hpIV;
            SetMonData(&gPlayerParty[monId], MON_DATA_HP_IV, &iv);
            iv = battlePokemon->attackIV;
            SetMonData(&gPlayerParty[monId], MON_DATA_ATK_IV, &iv);
            iv = battlePokemon->defenseIV;
            SetMonData(&gPlayerParty[monId], MON_DATA_DEF_IV, &iv);
            iv = battlePokemon->speedIV;
            SetMonData(&gPlayerParty[monId], MON_DATA_SPEED_IV, &iv);
            iv = battlePokemon->spAttackIV;
            SetMonData(&gPlayerParty[monId], MON_DATA_SPATK_IV, &iv);
            iv = battlePokemon->spDefenseIV;
            SetMonData(&gPlayerParty[monId], MON_DATA_SPDEF_IV, &iv);
            SetMonData(&gPlayerParty[monId], MON_DATA_PERSONALITY, &battlePokemon->personality);
            SetMonData(&gPlayerParty[monId], MON_DATA_STATUS, &battlePokemon->status1);
            SetMonData(&gPlayerParty[monId], MON_DATA_LEVEL, &battlePokemon->level);
            SetMonData(&gPlayerParty[monId], MON_DATA_HP, &battlePokemon->hp);
            SetMonData(&gPlayerParty[monId], MON_DATA_MAX_HP, &battlePokemon->maxHP);
            SetMonData(&gPlayerParty[monId], MON_DATA_ATK, &battlePokemon->attack);
            SetMonData(&gPlayerParty[monId], MON_DATA_DEF, &battlePokemon->defense);
            SetMonData(&gPlayerParty[monId], MON_DATA_SPEED, &battlePokemon->speed);
            SetMonData(&gPlayerParty[monId], MON_DATA_SPATK, &battlePokemon->spAttack);
            SetMonData(&gPlayerParty[monId], MON_DATA_SPDEF, &battlePokemon->spDefense);
        }
        break;
    case REQUEST_SPECIES_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPECIES, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_HELDITEM_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_HELD_ITEM, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_MOVES_PP_BATTLE:
        for (i = 0; i < 4; i++)
        {
            SetMonData(&gPlayerParty[monId], MON_DATA_MOVE1 + i, &moveData->moves[i]);
            SetMonData(&gPlayerParty[monId], MON_DATA_PP1 + i, &moveData->pp[i]);
        }
        SetMonData(&gPlayerParty[monId], MON_DATA_PP_BONUSES, &moveData->ppBonuses);
        break;
    case REQUEST_MOVE1_BATTLE:
    case REQUEST_MOVE2_BATTLE:
    case REQUEST_MOVE3_BATTLE:
    case REQUEST_MOVE4_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_MOVE1 + gBattleBufferA[gActiveBattler][1] - REQUEST_MOVE1_BATTLE, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_PP_DATA_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_PP1, &gBattleBufferA[gActiveBattler][3]);
        SetMonData(&gPlayerParty[monId], MON_DATA_PP2, &gBattleBufferA[gActiveBattler][4]);
        SetMonData(&gPlayerParty[monId], MON_DATA_PP3, &gBattleBufferA[gActiveBattler][5]);
        SetMonData(&gPlayerParty[monId], MON_DATA_PP4, &gBattleBufferA[gActiveBattler][6]);
        SetMonData(&gPlayerParty[monId], MON_DATA_PP_BONUSES, &gBattleBufferA[gActiveBattler][7]);
        break;
    case REQUEST_PPMOVE1_BATTLE:
    case REQUEST_PPMOVE2_BATTLE:
    case REQUEST_PPMOVE3_BATTLE:
    case REQUEST_PPMOVE4_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_PP1 + gBattleBufferA[gActiveBattler][1] - REQUEST_PPMOVE1_BATTLE, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_OTID_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_OT_ID, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_EXP_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_EXP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_HP_EV_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_HP_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_ATK_EV_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_ATK_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_DEF_EV_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_DEF_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SPEED_EV_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPEED_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SPATK_EV_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPATK_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SPDEF_EV_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPDEF_EV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_FRIENDSHIP_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_FRIENDSHIP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_POKERUS_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_POKERUS, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_MET_LOCATION_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_MET_LOCATION, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_MET_LEVEL_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_MET_LEVEL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_MET_GAME_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_MET_GAME, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_POKEBALL_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_POKEBALL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_ALL_IVS_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_HP_IV, &gBattleBufferA[gActiveBattler][3]);
        SetMonData(&gPlayerParty[monId], MON_DATA_ATK_IV, &gBattleBufferA[gActiveBattler][4]);
        SetMonData(&gPlayerParty[monId], MON_DATA_DEF_IV, &gBattleBufferA[gActiveBattler][5]);
        SetMonData(&gPlayerParty[monId], MON_DATA_SPEED_IV, &gBattleBufferA[gActiveBattler][6]);
        SetMonData(&gPlayerParty[monId], MON_DATA_SPATK_IV, &gBattleBufferA[gActiveBattler][7]);
        SetMonData(&gPlayerParty[monId], MON_DATA_SPDEF_IV, &gBattleBufferA[gActiveBattler][8]);
        break;
    case REQUEST_HP_IV_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_HP_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_ATK_IV_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_ATK_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_DEF_IV_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_DEF_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SPEED_IV_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPEED_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SPATK_IV_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPATK_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SPDEF_IV_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPDEF_IV, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_PERSONALITY_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_PERSONALITY, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_CHECKSUM_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_CHECKSUM, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_STATUS_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_STATUS, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_LEVEL_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_LEVEL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_HP_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_HP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_MAX_HP_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_MAX_HP, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_ATK_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_ATK, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_DEF_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_DEF, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SPEED_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPEED, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SPATK_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPATK, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SPDEF_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_SPDEF, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_COOL_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_COOL, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_BEAUTY_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_BEAUTY, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_CUTE_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_CUTE, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SMART_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_SMART, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_TOUGH_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_TOUGH, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SHEEN_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_SHEEN, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_COOL_RIBBON_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_COOL_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_BEAUTY_RIBBON_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_BEAUTY_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_CUTE_RIBBON_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_CUTE_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_SMART_RIBBON_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_SMART_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    case REQUEST_TOUGH_RIBBON_BATTLE:
        SetMonData(&gPlayerParty[monId], MON_DATA_TOUGH_RIBBON, &gBattleBufferA[gActiveBattler][3]);
        break;
    }

    HandleLowHpMusicChange(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
}

static void PlayerPartnerHandleSetRawMonData(void)
{
    u8 *dst = (u8 *)&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]] + gBattleBufferA[gActiveBattler][1];
    u8 i;

    for (i = 0; i < gBattleBufferA[gActiveBattler][2]; i++)
        dst[i] = gBattleBufferA[gActiveBattler][3 + i];

    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleLoadMonSprite(void)
{
    u16 species;

    BattleLoadPlayerMonSpriteGfx(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
    species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES);
    sub_806A068(species, GetBattlerPosition(gActiveBattler));

    gBattlerSpriteIds[gActiveBattler] = CreateSprite(&gUnknown_0202499C,
                                               GetBattlerSpriteCoord(gActiveBattler, 2),
                                               GetBattlerSpriteDefault_Y(gActiveBattler),
                                               sub_80A82E4(gActiveBattler));
    gSprites[gBattlerSpriteIds[gActiveBattler]].pos2.x = -240;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = gActiveBattler;
    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = gActiveBattler;
    StartSpriteAnim(&gSprites[gBattlerSpriteIds[gActiveBattler]], gBattleMonForms[gActiveBattler]);
    gBattlerControllerFuncs[gActiveBattler] = sub_81BB1D4;
}

static void PlayerPartnerHandleSwitchInAnim(void)
{
    ClearTemporarySpeciesSpriteData(gActiveBattler, gBattleBufferA[gActiveBattler][2]);
    gBattlerPartyIndexes[gActiveBattler] = gBattleBufferA[gActiveBattler][1];
    BattleLoadPlayerMonSpriteGfx(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
    sub_81BD0E4(gActiveBattler, gBattleBufferA[gActiveBattler][2]);
    gBattlerControllerFuncs[gActiveBattler] = sub_81BBAE8;
}

static void sub_81BD0E4(u8 bank, bool8 dontClearSubstituteBit)
{
    u16 species;

    ClearTemporarySpeciesSpriteData(bank, dontClearSubstituteBit);
    gBattlerPartyIndexes[bank] = gBattleBufferA[bank][1];
    species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[bank]], MON_DATA_SPECIES);
    gUnknown_03005D7C[bank] = CreateInvisibleSpriteWithCallback(sub_805D714);
    sub_806A068(species, GetBattlerPosition(bank));

    gBattlerSpriteIds[bank] = CreateSprite(
      &gUnknown_0202499C,
      GetBattlerSpriteCoord(bank, 2),
      GetBattlerSpriteDefault_Y(bank),
      sub_80A82E4(bank));

    gSprites[gUnknown_03005D7C[bank]].data[1] = gBattlerSpriteIds[bank];
    gSprites[gUnknown_03005D7C[bank]].data[2] = bank;

    gSprites[gBattlerSpriteIds[bank]].data[0] = bank;
    gSprites[gBattlerSpriteIds[bank]].data[2] = species;
    gSprites[gBattlerSpriteIds[bank]].oam.paletteNum = bank;

    StartSpriteAnim(&gSprites[gBattlerSpriteIds[bank]], gBattleMonForms[bank]);

    gSprites[gBattlerSpriteIds[bank]].invisible = TRUE;
    gSprites[gBattlerSpriteIds[bank]].callback = SpriteCallbackDummy;

    gSprites[gUnknown_03005D7C[bank]].data[0] = DoPokeballSendOutAnimation(0, POKEBALL_PLAYER_SENDOUT);
}

static void PlayerPartnerHandleReturnMonToBall(void)
{
    if (gBattleBufferA[gActiveBattler][1] == 0)
    {
        gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].animationState = 0;
        gBattlerControllerFuncs[gActiveBattler] = DoSwitchOutAnimation;
    }
    else
    {
        FreeSpriteOamMatrix(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        DestroySprite(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
        SetHealthboxSpriteInvisible(gHealthboxSpriteIds[gActiveBattler]);
        PlayerPartnerBufferExecCompleted();
    }
}

static void DoSwitchOutAnimation(void)
{
    switch (gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].animationState)
    {
    case 0:
        if (gBattleSpritesDataPtr->battlerData[gActiveBattler].behindSubstitute)
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, B_ANIM_SUBSTITUTE_TO_MON);

        gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].animationState = 1;
        break;
    case 1:
        if (!gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].specialAnimActive)
        {
            gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].animationState = 0;
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, B_ANIM_SWITCH_OUT_PLAYER_MON);
            gBattlerControllerFuncs[gActiveBattler] = sub_81BB828;
        }
        break;
    }
}

// some explanation here
// in emerald it's possible to have a tag battle in the battle frontier facilities with AI
// which use the front sprite for both the player and the partner as opposed to any other battles (including the one with Steven) that use the back pic as well as animate it
static void PlayerPartnerHandleDrawTrainerPic(void)
{
    s16 xPos, yPos;
    u32 trainerPicId;

    if (gPartnerTrainerId == STEVEN_PARTNER_ID)
    {
        trainerPicId = BACK_PIC_STEVEN;
        xPos = 90;
        yPos = (8 - gTrainerBackPicCoords[trainerPicId].coords) * 4 + 80;
    }
    else
    {
        trainerPicId = GetFrontierTrainerFrontSpriteId(gPartnerTrainerId);
        xPos = 32;
        yPos = (8 - gTrainerFrontPicCoords[trainerPicId].coords) * 4 + 80;
    }

    // Use back pic only if the partner is Steven
    if (gPartnerTrainerId == STEVEN_PARTNER_ID)
    {
        DecompressTrainerBackPic(trainerPicId, gActiveBattler);
        sub_806A12C(trainerPicId, GetBattlerPosition(gActiveBattler));
        gBattlerSpriteIds[gActiveBattler] = CreateSprite(&gUnknown_0202499C, xPos, yPos, sub_80A82E4(gActiveBattler));

        gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = gActiveBattler;
        gSprites[gBattlerSpriteIds[gActiveBattler]].pos2.x = 240;
        gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = -2;
        gSprites[gBattlerSpriteIds[gActiveBattler]].callback = sub_805D7AC;
    }
    else // otherwise use front sprite
    {
        DecompressTrainerFrontPic(trainerPicId, gActiveBattler);
        sub_806A1C0(trainerPicId, GetBattlerPosition(gActiveBattler));
        gBattlerSpriteIds[gActiveBattler] = CreateSprite(&gUnknown_0202499C, xPos, yPos, sub_80A82E4(gActiveBattler));

        gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = IndexOfSpritePaletteTag(gTrainerFrontPicPaletteTable[trainerPicId].tag);
        gSprites[gBattlerSpriteIds[gActiveBattler]].pos2.x = 240;
        gSprites[gBattlerSpriteIds[gActiveBattler]].pos2.y = 48;
        gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = -2;
        gSprites[gBattlerSpriteIds[gActiveBattler]].callback = sub_805D7AC;
        gSprites[gBattlerSpriteIds[gActiveBattler]].oam.affineMode = 0;
        gSprites[gBattlerSpriteIds[gActiveBattler]].hFlip = 1;
    }

    gBattlerControllerFuncs[gActiveBattler] = CompleteOnBankSpriteCallbackDummy;
}

static void PlayerPartnerHandleTrainerSlide(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleTrainerSlideBack(void)
{
    oamt_add_pos2_onto_pos1(&gSprites[gBattlerSpriteIds[gActiveBattler]]);
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = 35;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[2] = -40;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[4] = gSprites[gBattlerSpriteIds[gActiveBattler]].pos1.y;
    gSprites[gBattlerSpriteIds[gActiveBattler]].callback = sub_80A6EEC;
    StoreSpriteCallbackInData6(&gSprites[gBattlerSpriteIds[gActiveBattler]], SpriteCallbackDummy);
    gBattlerControllerFuncs[gActiveBattler] = sub_81BAE98;
}

static void PlayerPartnerHandleFaintAnimation(void)
{
    if (gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].animationState == 0)
    {
        if (gBattleSpritesDataPtr->battlerData[gActiveBattler].behindSubstitute)
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, B_ANIM_SUBSTITUTE_TO_MON);
        gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].animationState++;
    }
    else
    {
        if (!gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].specialAnimActive)
        {
            gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].animationState = 0;
            HandleLowHpMusicChange(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
            PlaySE12WithPanning(SE_POKE_DEAD, PAN_SIDE_PLAYER);
            gSprites[gBattlerSpriteIds[gActiveBattler]].data[1] = 0;
            gSprites[gBattlerSpriteIds[gActiveBattler]].data[2] = 5;
            gSprites[gBattlerSpriteIds[gActiveBattler]].callback = sub_8039C00;
            gBattlerControllerFuncs[gActiveBattler] = sub_81BB78C;
        }
    }
}

static void PlayerPartnerHandlePaletteFade(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleSuccessBallThrowAnim(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleBallThrowAnim(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandlePause(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleMoveAnimation(void)
{
    if (!mplay_80342A4(gActiveBattler))
    {
        u16 move = gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8);

        gAnimMoveTurn = gBattleBufferA[gActiveBattler][3];
        gAnimMovePower = gBattleBufferA[gActiveBattler][4] | (gBattleBufferA[gActiveBattler][5] << 8);
        gAnimMoveDmg = gBattleBufferA[gActiveBattler][6] | (gBattleBufferA[gActiveBattler][7] << 8) | (gBattleBufferA[gActiveBattler][8] << 16) | (gBattleBufferA[gActiveBattler][9] << 24);
        gAnimFriendship = gBattleBufferA[gActiveBattler][10];
        gWeatherMoveAnim = gBattleBufferA[gActiveBattler][12] | (gBattleBufferA[gActiveBattler][13] << 8);
        gAnimDisableStructPtr = (struct DisableStruct *)&gBattleBufferA[gActiveBattler][16];
        gTransformedPersonalities[gActiveBattler] = gAnimDisableStructPtr->transformedMonPersonality;
        if (IsMoveWithoutAnimation(move, gAnimMoveTurn)) // always returns FALSE
        {
            PlayerPartnerBufferExecCompleted();
        }
        else
        {
            gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].animationState = 0;
            gBattlerControllerFuncs[gActiveBattler] = PlayerPartnerDoMoveAnimation;
        }
    }
}

static void PlayerPartnerDoMoveAnimation(void)
{
    u16 move = gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8);
    u8 multihit = gBattleBufferA[gActiveBattler][11];

    switch (gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].animationState)
    {
    case 0:
        if (gBattleSpritesDataPtr->battlerData[gActiveBattler].behindSubstitute
            && !gBattleSpritesDataPtr->battlerData[gActiveBattler].flag_x8)
        {
            gBattleSpritesDataPtr->battlerData[gActiveBattler].flag_x8 = 1;
            InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, B_ANIM_SUBSTITUTE_TO_MON);
        }
        gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].animationState = 1;
        break;
    case 1:
        if (!gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].specialAnimActive)
        {
            sub_805EB9C(0);
            DoMoveAnim(move);
            gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].animationState = 2;
        }
        break;
    case 2:
        gAnimScriptCallback();
        if (!gAnimScriptActive)
        {
            sub_805EB9C(1);
            if (gBattleSpritesDataPtr->battlerData[gActiveBattler].behindSubstitute && multihit < 2)
            {
                InitAndLaunchSpecialAnimation(gActiveBattler, gActiveBattler, gActiveBattler, B_ANIM_MON_TO_SUBSTITUTE);
                gBattleSpritesDataPtr->battlerData[gActiveBattler].flag_x8 = 0;
            }
            gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].animationState = 3;
        }
        break;
    case 3:
        if (!gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].specialAnimActive)
        {
            CopyAllBattleSpritesInvisibilities();
            TrySetBehindSubstituteSpriteBit(gActiveBattler, gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8));
            gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].animationState = 0;
            PlayerPartnerBufferExecCompleted();
        }
        break;
    }
}

static void PlayerPartnerHandlePrintString(void)
{
    u16 *stringId;

    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    stringId = (u16*)(&gBattleBufferA[gActiveBattler][2]);
    BufferStringBattle(*stringId);
    BattleHandleAddTextPrinter(gDisplayedStringBattle, 0);
    gBattlerControllerFuncs[gActiveBattler] = CompleteOnInactiveTextPrinter2;
}

static void PlayerPartnerHandlePrintSelectionString(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleChooseAction(void)
{
    AI_TrySwitchOrUseItem();
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleUnknownYesNoBox(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleChooseMove(void)
{
    u8 chosenMoveId;
    struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct*)(&gBattleBufferA[gActiveBattler][4]);

    BattleAI_SetupAIData(0xF);
    chosenMoveId = BattleAI_ChooseMoveOrAction();

    if (gBattleMoves[moveInfo->moves[chosenMoveId]].target & (MOVE_TARGET_x10 | MOVE_TARGET_USER))
        gBattlerTarget = gActiveBattler;
    if (gBattleMoves[moveInfo->moves[chosenMoveId]].target & MOVE_TARGET_BOTH)
    {
        gBattlerTarget = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        if (gAbsentBattlerFlags & gBitTable[gBattlerTarget])
            gBattlerTarget = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
    }

    BtlController_EmitTwoReturnValues(1, 10, chosenMoveId | (gBattlerTarget << 8));
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleChooseItem(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleChoosePokemon(void)
{
    s32 chosenMonId = GetMostSuitableMonToSwitchInto();

    if (chosenMonId == 6) // just switch to the next mon
    {
        u8 playerMonIdentity = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        u8 selfIdentity = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);

        for (chosenMonId = 3; chosenMonId < 6; chosenMonId++)
        {
            if (GetMonData(&gPlayerParty[chosenMonId], MON_DATA_HP) != 0
                && chosenMonId != gBattlerPartyIndexes[playerMonIdentity]
                && chosenMonId != gBattlerPartyIndexes[selfIdentity])
            {
                break;
            }
        }
    }

    *(gBattleStruct->monToSwitchIntoId + gActiveBattler) = chosenMonId;
    BtlController_EmitChosenMonReturnValue(1, chosenMonId, NULL);
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleCmd23(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleHealthBarUpdate(void)
{
    s16 hpVal;

    LoadBattleBarGfx(0);
    hpVal = gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 8);

    if (hpVal != INSTANT_HP_BAR_DROP)
    {
        u32 maxHP = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MAX_HP);
        u32 curHP = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_HP);

        SetBattleBarStruct(gActiveBattler, gHealthboxSpriteIds[gActiveBattler], maxHP, curHP, hpVal);
    }
    else
    {
        u32 maxHP = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_MAX_HP);

        SetBattleBarStruct(gActiveBattler, gHealthboxSpriteIds[gActiveBattler], maxHP, 0, hpVal);
    }

    gBattlerControllerFuncs[gActiveBattler] = CompleteOnHealthbarDone;
}

static void PlayerPartnerHandleExpUpdate(void)
{
    u8 monId = gBattleBufferA[gActiveBattler][1];

    if (GetMonData(&gPlayerParty[monId], MON_DATA_LEVEL) >= MAX_MON_LEVEL)
    {
        PlayerPartnerBufferExecCompleted();
    }
    else
    {
        s16 expPointsToGive;
        u8 taskId;

        LoadBattleBarGfx(1);
        GetMonData(&gPlayerParty[monId], MON_DATA_SPECIES);  // unused return value
        expPointsToGive = gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 8);
        taskId = CreateTask(Task_GiveExpToMon, 10);
        gTasks[taskId].tExpTask_monId = monId;
        gTasks[taskId].tExpTask_gainedExp = expPointsToGive;
        gTasks[taskId].tExpTask_bank = gActiveBattler;
        gBattlerControllerFuncs[gActiveBattler] = nullsub_21;
    }
}

#undef tExpTask_monId
#undef tExpTask_gainedExp
#undef tExpTask_bank
#undef tExpTask_frames

static void PlayerPartnerHandleStatusIconUpdate(void)
{
    if (!mplay_80342A4(gActiveBattler))
    {
        u8 bank;

        UpdateHealthboxAttribute(gHealthboxSpriteIds[gActiveBattler], &gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], HEALTHBOX_STATUS_ICON);
        bank = gActiveBattler;
        gBattleSpritesDataPtr->healthBoxesData[bank].statusAnimActive = 0;
        gBattlerControllerFuncs[gActiveBattler] = CompleteOnFinishedStatusAnimation;
    }
}

static void PlayerPartnerHandleStatusAnimation(void)
{
    if (!mplay_80342A4(gActiveBattler))
    {
        InitAndLaunchChosenStatusAnimation(gBattleBufferA[gActiveBattler][1],
                        gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 8) | (gBattleBufferA[gActiveBattler][4] << 16) | (gBattleBufferA[gActiveBattler][5] << 24));
        gBattlerControllerFuncs[gActiveBattler] = CompleteOnFinishedStatusAnimation;
    }
}

static void PlayerPartnerHandleStatusXor(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleDataTransfer(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleDMA3Transfer(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandlePlayBGM(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleCmd32(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleTwoReturnValues(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleChosenMonReturnValue(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleOneReturnValue(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleOneReturnValue_Duplicate(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleCmd37(void)
{
    gUnknown_02022D0C.field_0 = 0;
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleCmd38(void)
{
    gUnknown_02022D0C.field_0 = gBattleBufferA[gActiveBattler][1];
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleCmd39(void)
{
    gUnknown_02022D0C.flag_x80 = 0;
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleCmd40(void)
{
    gUnknown_02022D0C.flag_x80 ^= 1;
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleHitAnimation(void)
{
    if (gSprites[gBattlerSpriteIds[gActiveBattler]].invisible == TRUE)
    {
        PlayerPartnerBufferExecCompleted();
    }
    else
    {
        gDoingBattleAnim = TRUE;
        gSprites[gBattlerSpriteIds[gActiveBattler]].data[1] = 0;
        DoHitAnimHealthboxEffect(gActiveBattler);
        gBattlerControllerFuncs[gActiveBattler] = DoHitAnimBlinkSpriteEffect;
    }
}

static void PlayerPartnerHandleCmd42(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandlePlaySE(void)
{
    s8 pan;

    if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
        pan = PAN_SIDE_PLAYER;
    else
        pan = PAN_SIDE_OPPONENT;

    PlaySE12WithPanning(gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8), pan);
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandlePlayFanfareOrBGM(void)
{
    if (gBattleBufferA[gActiveBattler][3])
    {
        BattleStopLowHpSound();
        PlayBGM(gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8));
    }
    else
    {
        PlayFanfare(gBattleBufferA[gActiveBattler][1] | (gBattleBufferA[gActiveBattler][2] << 8));
    }

    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleFaintingCry(void)
{
    u16 species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], MON_DATA_SPECIES);

    PlayCry3(species, -25, 5);
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleIntroSlide(void)
{
    HandleIntroSlide(gBattleBufferA[gActiveBattler][1]);
    gUnknown_020243FC |= 1;
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleIntroTrainerBallThrow(void)
{
    u8 paletteNum;
    u8 taskId;

    oamt_add_pos2_onto_pos1(&gSprites[gBattlerSpriteIds[gActiveBattler]]);

    gSprites[gBattlerSpriteIds[gActiveBattler]].data[0] = 50;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[2] = -40;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[4] = gSprites[gBattlerSpriteIds[gActiveBattler]].pos1.y;
    gSprites[gBattlerSpriteIds[gActiveBattler]].callback = sub_80A6EEC;
    gSprites[gBattlerSpriteIds[gActiveBattler]].data[5] = gActiveBattler;

    StoreSpriteCallbackInData6(&gSprites[gBattlerSpriteIds[gActiveBattler]], sub_805CC00);
    StartSpriteAnim(&gSprites[gBattlerSpriteIds[gActiveBattler]], 1);

    paletteNum = AllocSpritePalette(0xD6F9);
    if (gPartnerTrainerId == STEVEN_PARTNER_ID)
    {
        u8 spriteId = BACK_PIC_STEVEN;
        LoadCompressedPalette(gTrainerBackPicPaletteTable[spriteId].data, 0x100 + paletteNum * 16, 32);
    }
    else
    {
        u8 spriteId = GetFrontierTrainerFrontSpriteId(gPartnerTrainerId);
        LoadCompressedPalette(gTrainerFrontPicPaletteTable[spriteId].data, 0x100 + paletteNum * 16, 32);
    }


    gSprites[gBattlerSpriteIds[gActiveBattler]].oam.paletteNum = paletteNum;

    taskId = CreateTask(sub_81BE2C8, 5);
    gTasks[taskId].data[0] = gActiveBattler;

    if (gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].flag_x1)
        gTasks[gUnknown_020244B4[gActiveBattler]].func = sub_8073C30;

    gBattleSpritesDataPtr->animationData->field_9_x1 = 1;
    gBattlerControllerFuncs[gActiveBattler] = nullsub_77;
}

static void sub_81BE2C8(u8 taskId)
{
    if (gTasks[taskId].data[1] < 24)
    {
        gTasks[taskId].data[1]++;
    }
    else
    {
        u8 savedActiveBank = gActiveBattler;

        gActiveBattler = gTasks[taskId].data[0];
        if (!IsDoubleBattle() || (gBattleTypeFlags & BATTLE_TYPE_MULTI))
        {
            gBattleBufferA[gActiveBattler][1] = gBattlerPartyIndexes[gActiveBattler];
            sub_81BD0E4(gActiveBattler, FALSE);
        }
        else
        {
            gBattleBufferA[gActiveBattler][1] = gBattlerPartyIndexes[gActiveBattler];
            sub_81BD0E4(gActiveBattler, FALSE);
            gActiveBattler ^= BIT_FLANK;
            gBattleBufferA[gActiveBattler][1] = gBattlerPartyIndexes[gActiveBattler];
            BattleLoadPlayerMonSpriteGfx(&gPlayerParty[gBattlerPartyIndexes[gActiveBattler]], gActiveBattler);
            sub_81BD0E4(gActiveBattler, FALSE);
            gActiveBattler ^= BIT_FLANK;
        }
        gBattlerControllerFuncs[gActiveBattler] = sub_81BB02C;
        gActiveBattler = savedActiveBank;
        DestroyTask(taskId);
    }
}

static void PlayerPartnerHandleDrawPartyStatusSummary(void)
{
    if (gBattleBufferA[gActiveBattler][1] != 0 && GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
    {
        PlayerPartnerBufferExecCompleted();
    }
    else
    {
        gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].flag_x1 = 1;
        gUnknown_020244B4[gActiveBattler] = CreatePartyStatusSummarySprites(gActiveBattler, (struct HpAndStatus *)&gBattleBufferA[gActiveBattler][4], gBattleBufferA[gActiveBattler][1], gBattleBufferA[gActiveBattler][2]);
        gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].field_5 = 0;

        if (gBattleBufferA[gActiveBattler][2] != 0)
            gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].field_5 = 0x5D;

        gBattlerControllerFuncs[gActiveBattler] = sub_81BE498;
    }
}

static void sub_81BE498(void)
{
    if (gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].field_5++ > 0x5C)
    {
        gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].field_5 = 0;
        PlayerPartnerBufferExecCompleted();
    }
}

static void PlayerPartnerHandleCmd49(void)
{
    if (gBattleSpritesDataPtr->healthBoxesData[gActiveBattler].flag_x1)
        gTasks[gUnknown_020244B4[gActiveBattler]].func = sub_8073C30;
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleCmd50(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleSpriteInvisibility(void)
{
    if (IsBattlerSpritePresent(gActiveBattler))
    {
        gSprites[gBattlerSpriteIds[gActiveBattler]].invisible = gBattleBufferA[gActiveBattler][1];
        CopyBattleSpriteInvisibility(gActiveBattler);
    }
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleBattleAnimation(void)
{
    if (!mplay_80342A4(gActiveBattler))
    {
        u8 animationId = gBattleBufferA[gActiveBattler][1];
        u16 argument = gBattleBufferA[gActiveBattler][2] | (gBattleBufferA[gActiveBattler][3] << 8);

        if (TryHandleLaunchBattleTableAnimation(gActiveBattler, gActiveBattler, gActiveBattler, animationId, argument))
            PlayerPartnerBufferExecCompleted();
        else
            gBattlerControllerFuncs[gActiveBattler] = CompleteOnFinishedBattleAnimation;
    }
}

static void PlayerPartnerHandleLinkStandbyMsg(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleResetActionMoveSelection(void)
{
    PlayerPartnerBufferExecCompleted();
}

static void PlayerPartnerHandleCmd55(void)
{
    gBattleOutcome = gBattleBufferA[gActiveBattler][1];
    FadeOutMapMusic(5);
    BeginFastPaletteFade(3);
    PlayerPartnerBufferExecCompleted();
    gBattlerControllerFuncs[gActiveBattler] = sub_80587B0;
}

static void nullsub_128(void)
{
}
