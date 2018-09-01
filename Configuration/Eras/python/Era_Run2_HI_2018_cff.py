import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Modifier_run2_common_cff import run2_common
from Configuration.Eras.Modifier_run2_HI_specific_cff import run2_HI_specific
from Configuration.Eras.Modifier_stage1L1Trigger_cff import stage1L1Trigger
from Configuration.Eras.Modifier_run2_HI_2018_cff import run2_HI_2018
 
Run2_HI_2018 = cms.ModifierChain(run2_common, run2_HI_specific, stage1L1Trigger, run2_HI_2018)
