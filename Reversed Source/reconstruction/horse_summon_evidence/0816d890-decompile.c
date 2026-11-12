
long horse_dead_event(EVENT *event)

{
                    /* Unresolved local var: char_event_info * info@[DW_OP_reg0(EAX)] */
  CHARACTER::HorseSummon(*(CHARACTER **)event->info,false,false);
  return 0;
}

