This branch contains partial proofs for the function `xTaskCreate` in `tasks.c`
as well as proofs for functions called by `xTaskCreate`. Development of these
proofs was dropped when the focus shifted from `xTaskCreate` to
`vTaskSwitchContext`. The proofs reside in the directory
`Test/VeriFast/tasks/vTaskSwitchContext`.

They might be useful for anyone interested to continue
the verification of `xTaskCreate`. However, note that VeriFast recently
underwent some breaking changes that might break these proofs. In particular,
any attempt to reuse them as is will require turning off provenance checks
that have recently been introduced. This can be done by passing the flag
`-assume_no_provenance` to VeriFast.
