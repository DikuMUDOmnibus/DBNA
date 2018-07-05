#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

void do_eliminate( CHAR_DATA * ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument( argument, arg );

  if( arg[0] == '\0' )
  {
    send_to_char( "Spar whom?\n\r", ch );
    return;
  }

  if( ( victim = get_char_room( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  if( !IS_NPC( victim ) && !victim->desc )
  {
    send_to_char( "They are link dead, it wouldn't be right.", ch );
    return;
  }

  if( IS_NPC( victim ) )
  {
    send_to_char( "You can only spar other players.", ch );
    return;
  }

  if( victim == ch )
  {
    send_to_char( "You can't spar your self.\n\r", ch );
    return;
  }

  if( !IS_IMMORTAL( ch ) && !IS_NPC( ch ) && !IS_NPC( victim ) )
  {
    if( !xIS_SET( ch->act, PLR_QUESTING ) && xIS_SET( victim->act, PLR_QUESTING ) )
    {
      send_to_char( "You can't spar a player involved in a role playing event.\n\r", ch );
      return;
    }
    if( xIS_SET( ch->act, PLR_QUESTING ) && !xIS_SET( victim->act, PLR_QUESTING ) )
    {
      send_to_char( "You can't spar a player not involved in a role playing event.\n\r", ch );
      return;
    }
  }

  if( !xIS_SET( ch->act, PLR_SPAR ) )
    xSET_BIT( ch->act, PLR_SPAR );
  if( !xIS_SET( victim->act, PLR_SPAR ) )
    xSET_BIT( victim->act, PLR_SPAR );

  if( is_safe( ch, victim, TRUE ) )
    return;

  if( ch->position == POS_RESTING || ch->position == POS_SLEEPING || ch->position == POS_SITTING )
  {
    send_to_char( "How do you propose to do that in your current state?\n\r", ch );
    return;
  }

  if( ch->position == POS_FIGHTING
      || ch->position == POS_EVASIVE
      || ch->position == POS_DEFENSIVE || ch->position == POS_AGGRESSIVE || ch->position == POS_BERSERK )
  {
    send_to_char( "You do the best you can!\n\r", ch );
    return;
  }

  if( !xIS_SET( ch->in_room->room_flags, ROOM_ARENA ) )
  {
    send_to_char( "You must be in an arena to spar someone.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && !IS_NPC( victim ) && ch->exp <= 5000 )
  {
    send_to_char( "You can not fight other players while you are in training.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && !IS_NPC( victim ) && victim->exp <= 5000 )
  {
    send_to_char( "You can not fight other players while they are in training.\n\r", ch );
    return;
  }
  if( who_fighting( victim ) != NULL )
  {
    send_to_char( "It would not be honorable to interfere with some one else's battle.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && !IS_NPC( victim ) && victim->hit < 2 )
  {
    send_to_char( "They are too hurt to fight anymore.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && !IS_NPC( victim ) && ch->hit < 2 )
  {
    send_to_char( "You are too hurt to fight anymore.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && !IS_NPC( victim ) && !IS_HC( ch ) && !IS_HC( victim )
      && xIS_SET( ch->act, PLR_SPAR ) && xIS_SET( victim->act, PLR_SPAR )
      && ( ( IS_GOOD( ch ) && !IS_GOOD( victim ) )
           || ( IS_EVIL( ch ) && !IS_EVIL( victim ) ) || ( IS_NEUTRAL( ch ) && !IS_NEUTRAL( victim ) ) ) )
  {
    send_to_char( "You would not spar someone who is aligned that way.\n\r", ch );
    return;
  }

/*  if( !victim->pcdata->HBTCPartner )
  {
    send_to_char( "They are not accepting sparring partners at this time.\n\r", ch );
    return;
  }

  if( strcasecmp( ch->name, victim->pcdata->HBTCPartner ) )
  {
    send_to_char( "They do not want to spar with you.\n\r", ch );
    return;
  }*/
  WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
  if( !xIS_SET( ch->act, PLR_SPAR ) )
  {
    sprintf( buf, "Help!  I am being attacked by %s!", IS_NPC( ch ) ? ch->short_descr : ch->name );
    if( IS_PKILL( victim ) )
    {
      do_wartalk( victim, buf );
    }
    else
    {
      do_yell( victim, buf );
    }
  }
  check_illegal_pk( ch, victim );

  ch->spar_start = ch->exp;
  victim->spar_start = victim->exp;
  ch->delay = 0;
  check_attacker( ch, victim );
  multi_hit( ch, victim, TYPE_UNDEFINED );
  return;


}
