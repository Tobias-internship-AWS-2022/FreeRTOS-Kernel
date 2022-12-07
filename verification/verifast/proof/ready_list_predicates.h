#ifndef READY_LIST_PREDICATES_H
#define READY_LIST_PREDICATES_H

#include "single_core_proofs/scp_list_predicates.h"
#include "verifast_task_running_states.h"
#include "verifast_lists_extended.h"


/*@
predicate readyLists_p(list<void*> gTasks,
                       list<TaskRunning_t> gStates,
                       list<list<struct xLIST_ITEM*> > gCellLists,
                       list<list<void*> > gOwnerLists) =
    length(gTasks) == length(gStates) &*&
    length(gCellLists) == length(gOwnerLists) &*&
    forall(gOwnerLists, distinct) == true &*&
    configMAX_PRIORITIES == length(gCellLists) &*&
    List_array_p(&pxReadyTasksLists, configMAX_PRIORITIES, 
                 gCellLists, gOwnerLists)
    &*&
    // (IDLE) The ready list for priority level 0 contains an idle task for
    // this core.
        idleTask_p(?gIdleTask, coreID_f(), gTasks, gStates, ?gIdleTasks) &*&
        gIdleTasks == nth(0, gOwnerLists)
    &*&
    // List of priority 0 always contains the idle tasks (configNUM_CORES many) 
    // and the end marker nothing else
    length( nth(0, gCellLists) ) == configNUM_CORES + 1 &*&
    length( gIdleTasks ) == configNUM_CORES + 1;


predicate List_array_p(List_t* array, int size, 
                       list<list<struct xLIST_ITEM*> > cellLists,
                       list<list<void*> > ownerLists) =
    size >= 0 &*&
    length(cellLists) == size &*&
    length(ownerLists) == length(cellLists) &*&
    size > 0
        ? (
            cellLists == cons(?gCells, ?gTailCellLists) &*&
            ownerLists == cons(?gOwners, ?gTailOwnerLists) &*&
            pointer_within_limits(array) == true &*&
            xLIST(array, ?gLen, ?gIndex, ?gListEnd, gCells, ?gVals, 
                  gOwners)
            &*&
            gLen < INT_MAX &*&
            List_array_p(array + 1, size - 1, gTailCellLists, gTailOwnerLists)
        )
        : (
            cellLists == nil &*&
            ownerLists == nil
        );

lemma void List_array_size_positive(List_t* pxArray)
requires List_array_p(pxArray, ?gSize, ?gCellLists, ?gOwnerLists);
ensures  
    List_array_p(pxArray, gSize, gCellLists, gOwnerLists) &*&
    gSize >= 0 &*& 
    gSize == length(gCellLists) &*& 
    length(gCellLists) == length(gOwnerLists);
{
    open  List_array_p(pxArray, gSize, gCellLists, gOwnerLists);
    close List_array_p(pxArray, gSize, gCellLists, gOwnerLists);
}

lemma void List_array_split(List_t* array, int index)
requires 
    List_array_p(array, ?gSize, ?gCellLists, ?gOwnerLists) &*& 
    0 <= index &*& index < gSize;
ensures 
    List_array_p(array, index, ?gPrefCellLists, ?gPrefOwnerLists) &*&
    gPrefCellLists == take(index, gCellLists) &*&
    gPrefOwnerLists == take(index, gOwnerLists) &*&
    pointer_within_limits(array) == true &*&
    xLIST(array + index, ?gLen, _, _, ?gCells, ?gVals, ?gOwners) &*&
    gLen < INT_MAX &*&
    gCells == nth(index, gCellLists) &*&
    gOwners == nth(index, gOwnerLists) &*&
    mem(gOwners, gOwnerLists) == true &*&
    List_array_p(array + index + 1, gSize-index-1, ?gSufCellLists, ?gSufOwnerLists) &*&
    gSufCellLists == drop(index+1, gCellLists) &*&
    gSufOwnerLists == drop(index+1, gOwnerLists);
{
    open List_array_p(array, gSize, gCellLists, gOwnerLists);

    if( index > 0 ) {
        List_array_split(array + 1, index - 1);
    }

    close List_array_p(array, index, take(index, gCellLists), take(index, gOwnerLists));
}

lemma void List_array_join(List_t* array)
requires
    List_array_p(array, ?gPrefSize, ?gPrefCellLists, ?gPrefOwnerLists) &*&
    xLIST(array + gPrefSize, ?gLen, _, _, ?gCells, _, ?gOwners) &*&
    gLen < INT_MAX &*&
    pointer_within_limits(array + gPrefSize) == true &*&
    List_array_p(array + gPrefSize + 1, ?gSufSize, ?gSufCellLists, ?gSufOwnerLists);
ensures 
    List_array_p(array, ?gSize, ?gCellLists, ?gOwnerLists) &*& 
    gSize == length(gCellLists) &*&
    length(gCellLists) == length(gOwnerLists) &*&
    gSize == gPrefSize + 1 + gSufSize &*&
    gCellLists == append(gPrefCellLists, cons(gCells, gSufCellLists)) &*&
    gOwnerLists == append(gPrefOwnerLists, cons(gOwners, gSufOwnerLists));
{
    open List_array_p(array, gPrefSize, gPrefCellLists, gPrefOwnerLists);
    List_array_size_positive(array + gPrefSize + 1);

    if( gPrefSize > 0 ) {
        List_array_join(array + 1);
    }

    close List_array_p(array, gPrefSize + 1 + gSufSize,
                       append(gPrefCellLists, cons(gCells, gSufCellLists)),
                       append(gPrefOwnerLists, cons(gOwners, gSufOwnerLists)));
}

lemma void List_array_p_index_within_limits(List_t* array, int index)
requires List_array_p(array, ?gSize, ?gCellLists, ?gOwnerLists) &*&
         0 <= index &*& index < gSize;
ensures List_array_p(array, gSize, gCellLists, gOwnerLists) &*&
        pointer_within_limits(&array[index]) == true;
{
    open List_array_p(array, gSize, gCellLists, gOwnerLists);
    if( index > 0) {
        List_array_p_index_within_limits(&array[1], index-1);
    }
    close List_array_p(array, gSize, gCellLists, gOwnerLists);
}


predicate idleTask_p(TCB_t* t, int coreID, 
                     list<void*> gTasks, 
                     list<TaskRunning_t> gStates,
                     list<void*> gIdleTasks) =
    0 <= coreID &*& coreID < configNUM_CORES &*&
    mem(t, gTasks) == true &*&
    mem(t, gIdleTasks) == true &*&
    length(gStates) == length(gTasks)
    &*&
    // The idle task for this core is either not running or running on this core.
    (nth(index_of(t, gTasks), gStates) == taskTASK_NOT_RUNNING ||
        nth(index_of(t, gTasks), gStates) == coreID
    );
@*/


// -------------------------------------------------------------------------
// Lemmas to close the ready list predicate in different scenarios.
/*@
lemma void closeUnchanged_readyLists(list<void*> tasks,
                                     list<TaskRunning_t> states,
                                     list<list<struct xLIST_ITEM*> > cellLists,
                                     list<list<void*> > ownerLists)
requires 
    length(tasks) == length(states) &*&
    configMAX_PRIORITIES == length(cellLists) &*&
    configMAX_PRIORITIES == length(ownerLists) &*&
    length( nth(0, cellLists) ) == configNUM_CORES +1 &*&
    length( nth(0, ownerLists) ) == configNUM_CORES +1 &*&
    forall(ownerLists, distinct) == true &*&
    List_array_p(&pxReadyTasksLists, ?gIndex, ?gPrefCellLists, ?gPrefOwnerLists) &*&
    gIndex < length(cellLists) &*&
    xLIST(&pxReadyTasksLists + gIndex, ?gLen, _, _, ?gCells, ?gVals, ?gOwners) &*&
    gLen < INT_MAX &*&
    gCells == nth(gIndex, cellLists) &*&
    gOwners == nth(gIndex, ownerLists) &*&
    pointer_within_limits(&pxReadyTasksLists + gIndex) == true &*&
    List_array_p(&pxReadyTasksLists + gIndex + 1, configMAX_PRIORITIES - gIndex - 1,
                 ?gSufCellLists, ?gSufOwnerLists) &*&
    gPrefCellLists == take(gIndex, cellLists) &*&
    gSufCellLists == drop(gIndex+1, cellLists) &*&
    gPrefOwnerLists == take(gIndex, ownerLists) &*&
    gSufOwnerLists == drop(gIndex+1, ownerLists) &*&
    idleTask_p(?gIdleTask, coreID_f(), tasks, states, nth(0, ownerLists));
ensures
    readyLists_p(tasks, states, cellLists, ownerLists);
{
    // Prove `0 <= gIndex`:
        open List_array_p(&pxReadyTasksLists, gIndex, gPrefCellLists, gPrefOwnerLists);
        close List_array_p(&pxReadyTasksLists, gIndex, gPrefCellLists, gPrefOwnerLists);
    assert( 0 <= gIndex );
    
    List_array_join(&pxReadyTasksLists);
    assert( List_array_p(&pxReadyTasksLists, ?gSize, ?gCellLists2, ?gOwnerLists2) );
    
    append_take_nth_drop(gIndex, cellLists);
    append_take_nth_drop(gIndex, ownerLists);
    assert( gSize == configMAX_PRIORITIES );
    assert( gCellLists2 == cellLists );
    assert( gOwnerLists2 == ownerLists );

    close readyLists_p(tasks, states, cellLists, ownerLists);
}

lemma void closeReordered_readyLists(list<void*> tasks, 
                                     list<TaskRunning_t> states,
                                     list<list<struct xLIST_ITEM*> > cellLists,
                                     list<list<void*> > ownerLists,
                                     list<struct xLIST_ITEM*> reorderedCells,
                                     list<void*> reorderedOwners)
requires
    length(tasks) == length(states) &*&
    configMAX_PRIORITIES == length(cellLists) &*&
    configMAX_PRIORITIES == length(ownerLists) &*&
    length( nth(0, cellLists) ) == configNUM_CORES + 1 &*&
    length( nth(0, ownerLists) ) == configNUM_CORES + 1 &*&
    forall(ownerLists, distinct) == true &*&
    List_array_p(&pxReadyTasksLists, ?gIndex, ?gPrefCellLists, ?gPrefOwnerLists) &*&
    gIndex < length(cellLists) &*&
    xLIST(&pxReadyTasksLists + gIndex, ?gLen, _, _, reorderedCells, _, reorderedOwners) &*&
    gLen < INT_MAX &*&
    length(reorderedCells) == length(nth(gIndex, cellLists)) &*&
    length(reorderedOwners) == length(nth(gIndex, ownerLists)) &*&
    distinct(reorderedOwners) == true &*&
    pointer_within_limits(&pxReadyTasksLists + gIndex) == true &*&
    List_array_p(&pxReadyTasksLists + gIndex + 1, configMAX_PRIORITIES - gIndex - 1,
                 ?gSufCellLists, ?gSufOwnerLists) &*&
    gPrefCellLists == take(gIndex, cellLists) &*&
    gSufCellLists == drop(gIndex+1, cellLists) &*&
    gPrefOwnerLists == take(gIndex, ownerLists) &*&
    gSufOwnerLists == drop(gIndex+1, ownerLists) &*&
    forall(ownerLists, (superset)(tasks)) == true 
    &*&
//    forall(reorderedOwners, (mem_list_elem)(tasks)) == true &*&
    // `reorderedOwners` and original owner list `nth(gIndex, ownerLists)`
    // contain same elements 
        subset(reorderedOwners, nth(gIndex, ownerLists)) == true &*&
        subset(nth(gIndex, ownerLists), reorderedOwners) == true
    &*&
    subset(nth(gIndex, ownerLists), tasks) == true &*&
    idleTask_p(?gIdleTask, coreID_f(), tasks, states, nth(0, ownerLists));
ensures
    readyLists_p(tasks, states, ?gReorderedCellLists, ?gReorderedOwnerLists) &*&
    forall(gReorderedOwnerLists, (superset)(tasks)) == true;
{
    // Prove that `gIndex != 0 -> gIndex > 0`
    if(gIndex != 0) {
        open List_array_p(&pxReadyTasksLists, gIndex, gPrefCellLists, gPrefOwnerLists);
        close List_array_p(&pxReadyTasksLists, gIndex, gPrefCellLists, gPrefOwnerLists);
        assert( gIndex > 0 );
    }

    List_array_join(&pxReadyTasksLists);
    assert( List_array_p(&pxReadyTasksLists, configMAX_PRIORITIES, 
                         ?gReorderedCellLists, ?gReorderedOwnerLists) );
    
    // Proving that joining preserves distinctness:
        assert( gReorderedOwnerLists == append(gPrefOwnerLists, 
                                               cons(reorderedOwners, gSufOwnerLists)) );
        forall_drop(ownerLists, distinct, gIndex+1);
        assert( forall(gSufOwnerLists, distinct) == true );
        forall_take(ownerLists, distinct, gIndex);
        assert( forall(gPrefOwnerLists, distinct) == true );
        assert( forall(cons(reorderedOwners, gSufOwnerLists), distinct) == true );
        forall_append(gPrefOwnerLists, cons(reorderedOwners, gSufOwnerLists), distinct);
    assert( forall(gReorderedOwnerLists, distinct) == true );


    // Proving `length(nth(0, gReorderedCellLists)) == configNUM_CORES + 1
    //          == length(nth(0, gReorderedCellLists))`
        if(gIndex == 0) {
            assert( nth(0, gReorderedCellLists) == reorderedCells );
            assert( nth(0, gReorderedOwnerLists) == reorderedOwners );
        } else {
            nth_take(0, gIndex, cellLists);
            assert( nth(0, gReorderedCellLists) == nth(0, gPrefCellLists) );
            assert( nth(0, gPrefCellLists) == nth(0, cellLists) );

            nth_take(0, gIndex, ownerLists);
            assert( nth(0, gReorderedOwnerLists) == nth(0, gPrefOwnerLists) );
            assert( nth(0, gPrefOwnerLists) == nth(0, ownerLists) );
        }
    assert( length(nth(0, gReorderedCellLists)) == configNUM_CORES + 1 );
    assert( length(nth(0, gReorderedOwnerLists)) == configNUM_CORES + 1 );

    open idleTask_p(gIdleTask, coreID_f(), tasks, states, ?gIdleTasks);
    list<void*> gReorderedIdleTasks = nth(0, gReorderedOwnerLists);

    // Proving `gIdleTask ∈ gReorderedIdleTasks
        mem_subset(gIdleTask, gIdleTasks, gReorderedIdleTasks);
    assert( mem(gIdleTask, gReorderedIdleTasks) == true );

    close idleTask_p(gIdleTask, coreID_f(), tasks, states, gReorderedIdleTasks);
    close readyLists_p(tasks, states, gReorderedCellLists, gReorderedOwnerLists);


    // Proving `tasks ⊇ reorderedOwners`:
        subset_trans(reorderedOwners, nth(gIndex, ownerLists), tasks);
        assert( subset(reorderedOwners, tasks) == true );
    assert( superset(tasks, reorderedOwners) == true );

    // Proving `∀l ∈ gReorderedOwnerLists. tasks ⊇ l`:
        forall_take(ownerLists, (superset)(tasks), gIndex);
        forall_drop(ownerLists, (superset)(tasks), gIndex+1);
        assert( forall(gPrefOwnerLists, (superset)(tasks)) == true );
        assert( forall(gSufOwnerLists, (superset)(tasks)) == true );
        assert( superset(tasks, reorderedOwners) == true );
        assert( forall(singleton(reorderedOwners), (superset)(tasks)) == true );
        assert( forall(cons(reorderedOwners, gSufOwnerLists), (superset)(tasks)) == true );

        forall_append(gPrefOwnerLists, cons(reorderedOwners, gSufOwnerLists),
                    (superset)(tasks));
    assert( forall(gReorderedOwnerLists, (superset)(tasks)) == true );
}
@*/


/*@
predicate VF_reorderReadyList__ghost_args(list<void*> tasks,
                                         list<TaskRunning_t> states,
                                         list<list<struct xLIST_ITEM*> > cellLists,
                                         list<list<void*> > ownerLists,
                                         int offset) 
    = true;
@*/

void VF_reorderReadyList(List_t* pxReadyList, ListItem_t * pxTaskItem)
/*@ requires
        // ghost arguments
            VF_reorderReadyList__ghost_args(?gTasks, ?gStates,
                                           ?gCellLists, ?gOwnerLists, ?gOffset)
            &*&
            length(gTasks) == length(gStates) &*&
            length(gCellLists) == configMAX_PRIORITIES &*&
            length(gOwnerLists) == configMAX_PRIORITIES &*&
            length(nth(0, gCellLists)) == configNUM_CORES + 1 &*&
            length(nth(0, gOwnerLists)) == configNUM_CORES + 1 &*&
            forall(gOwnerLists, distinct) == true &*&
            0 <= gOffset &*& gOffset < length(gCellLists) 
        &*&
        // current ready list
            xLIST(pxReadyList, ?gSize, ?gIndex, ?gEnd, ?gCells, ?gVals, ?gOwners) &*&
            pxReadyList == &pxReadyTasksLists + gOffset &*&
            pointer_within_limits(pxReadyList) == true &*&
            gSize < INT_MAX &*&
            gEnd != pxTaskItem &*&
            mem(pxTaskItem, gCells) == true &*&
            gCells == nth(gOffset, gCellLists) &*&
            gOwners == nth(gOffset, gOwnerLists) &*&
            distinct(gOwners) == true &*&
            idleTask_p(?gIdleTask, coreID_f(), gTasks, gStates, ?gIdleTasks) &*&
            gIdleTasks == nth(0, gOwnerLists)
        &*&
        // prefix and suffix of ready lists array
            List_array_p(&pxReadyTasksLists, gOffset, ?gPrefCellLists, ?gPrefOwnerLists) &*&
            List_array_p(&pxReadyTasksLists + gOffset + 1, configMAX_PRIORITIES - gOffset - 1,
                        ?gSufCellLists, ?gSufOwnerLists)
            &*&
            gPrefCellLists == take(gOffset, gCellLists) &*&
            gSufCellLists == drop(gOffset+1, gCellLists) &*&
            gPrefOwnerLists == take(gOffset, gOwnerLists) &*&
            gSufOwnerLists == drop(gOffset+1, gOwnerLists) &*&
            forall(gOwnerLists, (superset)(gTasks)) == true &*&
            subset(gOwners, gTasks) == true;
@*/
/*@ ensures 
        readyLists_p(gTasks, gStates, ?gReorderedCellLists, ?gReorderedOwnerLists) &*&
        length(gReorderedCellLists) == length(gCellLists) &*&
        length(gReorderedOwnerLists) == length(gOwnerLists) &*&
        length(gReorderedCellLists) == length(gReorderedOwnerLists) &*&
        forall(gReorderedOwnerLists, (superset)(gTasks)) == true;
 @*/
{
    //@ open VF_reorderReadyList__ghost_args(_, _, _, _, _);

    // Proving `∀o ∈ gOwners. o ∈ gTasks`
        //@ forall_mem(gOwners, gOwnerLists, (superset)(gTasks));
        //@ assert( superset(gTasks, gOwners) == true );
        //@ subset_implies_forall_mem(gOwners, gTasks);
    //@ assert( forall(gOwners, (mem_list_elem)(gTasks)) == true );

    // Proving `length(gCells) == length(gOwners) == gSize + 1`:
        //@ open xLIST(pxReadyList, gSize, gIndex, gEnd, gCells, gVals, gOwners);
        //@ close xLIST(pxReadyList, gSize, gIndex, gEnd, gCells, gVals, gOwners);
    //@ assert( length(gCells) == length(gOwners) );
    //@ assert( length(gCells) == gSize +1 );

    //@ close exists(pxReadyList);
    uxListRemove( pxTaskItem );
    //@ assert( xLIST(pxReadyList, gSize-1, ?gIndex2, gEnd, ?gCells2, ?gVals2, ?gOwners2) );
    //@ assert( xLIST_ITEM(pxTaskItem, _, _, _, ?gTaskItem_owner, _) );

    // Proving `gOwners2 == remove(gTaskItem_owner, gOwners)`:
        //@ assert( gOwners2 == remove_nth(index_of(pxTaskItem, gCells), gOwners) );
        //@ assert( gTaskItem_owner == nth(index_of(pxTaskItem, gCells), gOwners) );
        //@ nth_index_of(index_of(pxTaskItem, gCells), gOwners);
        //@ remove_nth_index(gTaskItem_owner, gOwners);
        //@ assert( index_of(pxTaskItem, gCells) == index_of(gTaskItem_owner, gOwners) );
    //@ assert( gOwners2 == remove(gTaskItem_owner, gOwners) );

    // Proving `gOwners2 ⊆ gOwners`:
        //@ remove_result_subset(gTaskItem_owner, gOwners);
    //@ assert( subset(gOwners2, gOwners) == true );

    //@ distinct_remove(gTaskItem_owner, gOwners);
    //@ assert( distinct(gOwners2) == true );

    // gTaskItem_owner ∉ gOwners2
    //@ distinct_mem_remove(gTaskItem_owner, gOwners);
    //@ assert( mem(gTaskItem_owner, gOwners2) == false);


    // TODO: Which of the subproofs below are still necessary?
    // --------------------------------------------------------


    // Proving `length(gCell2) == length(gOwners2) == gSize` and `gIndex2 ∈ gCells2`:
        //@ open xLIST(pxReadyList, gSize-1, gIndex2, gEnd, gCells2, gVals2, gOwners2);
        //@ close xLIST(pxReadyList, gSize-1, gIndex2, gEnd, gCells2, gVals2, gOwners2);
    //@ assert( length(gCells2) == gSize );
    //@ assert( length(gOwners2) == gSize );
    //@ assert( mem(gIndex2, gCells2) == true );

    // Proving `gTaskItem_owner ∈ gOwners`:
        //@ assert( gTaskItem_owner == nth(index_of(pxTaskItem, gCells), gOwners) );
        //@ mem_index_of(pxTaskItem, gCells);
        //@ nth_implies_mem(index_of(pxTaskItem, gCells), gOwners);
    //@ assert( mem(gTaskItem_owner, gOwners) == true );

    // Proving `gTaskItem_owner ∈ gTasks`:
        //@ forall_mem(gTaskItem_owner, gOwners, (mem_list_elem)(gTasks));
    //@ assert( mem(gTaskItem_owner, gTasks) == true );

    // Proving `gOwners2 ⊆ gTasks` 
        //@ assert( forall(gOwners, (mem_list_elem)(gTasks)) == true );
        //@ forall_remove_nth(index_of(pxTaskItem, gCells), gOwners, (mem_list_elem)(gTasks));
        //@ assert( forall(gOwners2, (mem_list_elem)(gTasks)) == true );
        //@ forall_mem_implies_subset(gOwners2, gTasks);
    //@ assert( subset(gOwners2, gTasks) == true );

    vListInsertEnd( pxReadyList, pxTaskItem );
    //@ assert( xLIST(pxReadyList, gSize, ?gIndex3, gEnd, ?gCells3, ?gVals3, ?gOwners3) );


    // Proving `gOwners3 ⊆ gOwners ∧ gOwners ⊆ gOwners3` and
    //         `distinct(gOwners3) == true`:
    // We must handle the case split introduced by the postcondition of `vListInsertEnd`.
        /*@
        if( gIndex2 == gEnd ) {
            // postcondition of `vListInsertEnd`:
                assert( gCells3 == append(gCells2, singleton(pxTaskItem)) );
                assert( gOwners3 == append(gOwners2, singleton(gTaskItem_owner)) );

            assert( gOwners2 == remove(gTaskItem_owner, gOwners) );
            assert( gOwners3 == append( remove(gTaskItem_owner, gOwners),
                                        singleton(gTaskItem_owner) ) );

            append_remove_x_preserves_elems(gTaskItem_owner, gOwners);
            assert( subset(gOwners3, gOwners) == true );
            assert( subset(gOwners, gOwners3) == true );
            
            distinct_append_r(gTaskItem_owner, gOwners2);
            assert( distinct(gOwners3) == true );
        } else {
            // postcondition of `vListInsertEnd`:
                int i = index_of(gIndex2, gCells2);
                assert( gCells3 == append(take(i, gCells2),
                                        append(singleton(pxTaskItem), 
                                                drop(i, gCells2))) );
                list<void*> ot = append(singleton(gTaskItem_owner), drop(i, gOwners2));
                assert( gOwners3 == append(take(i, gOwners2), ot) );
            
            assert( gOwners3 == append(take(i, remove(gTaskItem_owner, gOwners)), 
                                        append(singleton(gTaskItem_owner), 
                                                drop(i, remove(gTaskItem_owner, gOwners)))
                                        )
                    );
            
            // Proving  `{TaskItem_owner} u gOwners2 ⊆ gOwners3` and
            //          `gOwners3 ⊆ {TaskItem_owner} u gOwners2`:
            mem_index_of(gIndex2, gCells2);
            append_take_elem_drop_adds_elem(i, gOwners2, gTaskItem_owner, gOwners3);
            assert( subset( cons(gTaskItem_owner, gOwners2), gOwners3) == true );
            assert( subset( gOwners3, cons(gTaskItem_owner, gOwners2)) == true );

            // {gTaskItem_owner} u gOwners2 ⊆ gOwners
            assert( subset(cons(gTaskItem_owner, gOwners2), gOwners) == true );

            // Proving `gOwners ⊆ {TaskItem_owner} u gOwners2`:
                cons_remove_x_preserves_elems(gTaskItem_owner, gOwners);
            assert( subset(gOwners, cons(gTaskItem_owner, gOwners2)) == true );
            
            // Proving `gOwners ⊆ {TaskItem_owner} u gOwners2 ⊆ gOwners3`
                subset_trans(gOwners, cons(gTaskItem_owner, gOwners2), gOwners3);
            assert( subset(gOwners, gOwners3) == true );

            // Proving `gOwners3 ⊆ {gTaskItem_owner} u gOwners2 ⊆ gOwners`:
                subset_trans(gOwners3, cons(gTaskItem_owner, gOwners2), gOwners);
            assert( subset(gOwners3, gOwners) == true );

            // Proving `distinct(gOwners2) == true`:
                distinct_take_elem_drop(i, gOwners2, gTaskItem_owner, gOwners3);
            assert( distinct(gOwners3) == true );
        }
        @*/
    //@ assert( subset(gOwners3, gOwners) == true );
    //@ assert( subset(gOwners, gOwners3) == true );
    //@ assert( distinct(gOwners3) == true );


    // Proving `gOwners3 ⊆ gTasks` and `length(gOwners3) == length(gOwners)`:
        //@ subset_trans(gOwners3, gOwners, gTasks);
    //@ assert( subset(gOwners3, gTasks) == true );
    //@ assert( length(gOwners3) == length(gOwners) );

    //@ subset_implies_forall_mem(gOwners3, gTasks);
    //@ assert( forall(gOwners3, (mem_list_elem)(gTasks)) == true );

    //@ closeReordered_readyLists(gTasks, gStates, gCellLists, gOwnerLists, gCells3, gOwners3);

    // Proving that reordering preserves the length of cell lists and owner lists:
        //@ open readyLists_p(gTasks, gStates, ?gReorderedCellLists, ?gReorderedOwnerLists);
        //@ close readyLists_p(gTasks, gStates, gReorderedCellLists, gReorderedOwnerLists);
    //@ assert( length(gReorderedCellLists) == length(gCellLists) );
    //@ assert( length(gReorderedOwnerLists) == length(gOwnerLists) );
    //@ assert( length(gReorderedCellLists) == length(gReorderedOwnerLists) );
}

// -------------------------------------------------------------------------
// Lemmas to update the idle task list predicate in different scenarios.
/*@
lemma void updateStop_idleTask(TCB_t* idleTask, 
                               list<TaskRunning_t> states,
                               TCB_t* stoppedTask,
                               list<TaskRunning_t> states1)
requires 
    idleTask_p(idleTask, coreID_f(), ?gTasks, states, ?gIdleTasks) &*&
    states1 == update(index_of(stoppedTask, gTasks), taskTASK_NOT_RUNNING, states) &*&
    mem(stoppedTask, gTasks) == true;
ensures
    idleTask_p(idleTask, coreID_f(), gTasks, states1, gIdleTasks);
{
    open idleTask_p(idleTask, coreID_f(), gTasks, states, gIdleTasks);

    int idxS = index_of(stoppedTask, gTasks);
    int idxI = index_of(idleTask, gTasks);

    if( stoppedTask == idleTask ) {
        mem_index_of(stoppedTask, gTasks);
        nth_update(idxS, idxS, taskTASK_NOT_RUNNING, states);
        assert( nth(idxS, states1) == taskTASK_NOT_RUNNING );
        assert( nth(idxS, states1) == taskTASK_NOT_RUNNING );
    } else {
        mem_index_of(stoppedTask, gTasks);
        index_of_different(stoppedTask, idleTask, gTasks);
        nth_update(idxI, idxS, taskTASK_NOT_RUNNING, states);
        assert( nth(idxI, states1) == nth(idxI, states) );
    }

    close idleTask_p(idleTask, coreID_f(), gTasks, states1, gIdleTasks);
}

lemma void updateStart_idleTask(TCB_t* idleTask, 
                               list<TaskRunning_t> states1,
                               TCB_t* startedTask,
                               list<TaskRunning_t> states2)
requires 
    idleTask_p(idleTask, coreID_f(), ?gTasks, states1, ?gIdleTasks) &*&
    states2 == update(index_of(startedTask, gTasks), coreID_f(), states1) &*&
    mem(startedTask, gTasks) == true;
ensures
    idleTask_p(idleTask, coreID_f(), gTasks, states2, gIdleTasks);
{
    open idleTask_p(idleTask, coreID_f(), gTasks, states1, gIdleTasks);

    int idxS = index_of(startedTask, gTasks);
    int idxI = index_of(idleTask, gTasks);

    if( startedTask == idleTask ) {
        mem_index_of(startedTask, gTasks);
        nth_update(idxS, idxS, coreID_f(), states1);
        assert( nth(idxS, states2) == coreID_f() );
        assert( nth(idxS, states2) == coreID_f() );
    } else {
        mem_index_of(startedTask, gTasks);
        index_of_different(startedTask, idleTask, gTasks);
        nth_update(idxI, idxS, coreID_f(), states1);
        assert( nth(idxI, states2) == nth(idxI, states1) );
    }

    close idleTask_p(idleTask, coreID_f(), gTasks, states2, gIdleTasks);
}

lemma void updateStopStart_idleTask(TCB_t* idleTask, 
                               list<TaskRunning_t> states,
                               TCB_t* stoppedTask,
                               TCB_t* startedTask,
                               list<TaskRunning_t> states2)
requires 
    idleTask_p(idleTask, coreID_f(), ?gTasks, states, ?gIdleTasks) &*&
    states2 == update(index_of(startedTask, gTasks), coreID_f(),
                      update(index_of(stoppedTask, gTasks), taskTASK_NOT_RUNNING, 
                             states))
    &*&
    mem(stoppedTask, gTasks) == true &*&
    mem(startedTask, gTasks) == true;
ensures
    idleTask_p(idleTask, coreID_f(), gTasks, states2, gIdleTasks);
{
    list<TaskRunning_t> states1 = 
        update(index_of(stoppedTask, gTasks), taskTASK_NOT_RUNNING, states);
    updateStop_idleTask(idleTask, states, stoppedTask, states1);
    updateStart_idleTask(idleTask, states1, startedTask, states2);
}
@*/
#endif /* READY_LIST_PREDICATES_H */