###For a description of the crabConfig.py parameters. See:
###https://twiki.cern.ch/twiki/bin/view/CMSPublic/CRAB3ConfigurationFile

from WMCore.Configuration import Configuration
config = Configuration() ###create a Configuration object

config.section_('General')###add a new section of type "General"
###General: In this section, the user specifies generic parameters about the request (e.g. request name).
config.General.workArea     = 'crab_Projects_Pixel_VS_General_tracks' ###fixed name for projects dir in my area

config.General.requestName  = 'HIMinimumBias1_HIRun2018A-PromptReco-v1_AOD_GeneralTracks'

config.General.transferLogs = True 
config.General.transferOutputs = True

################################

config.section_('JobType')###add a new section of type "JobType"
###JobType: This section aims to contain all the parameters of the user job type and 
###related configurables (e.g. CMSSW parameter-set configuration file, additional input files, etc.).
config.JobType.pluginName     = 'Analysis'
config.JobType.psetName       = 'run_PbPb_DATA_trackVariables_WithEventSel_cfg.py'
config.JobType.maxMemoryMB    = 2500


################################

config.section_('Data')###add a new section of type "Data"
###Data: This section contains all the parameters related to the data to be analyzed, 
###including the splitting parameters.
config.Data.inputDataset      = '/HIMinimumBias1/HIRun2018A-PromptReco-v1/AOD'
config.Data.splitting         = 'FileBased'
config.Data.unitsPerJob       = 1
config.Data.totalUnits        = 50
config.Data.inputDBS          = 'global'
config.Data.outLFNDirBase            = '/store/user/caber/HIMinimumBias1_HIRun2018A-PromptReco-v1_AOD_GeneralTracks'
config.Data.runRange = '326586'


################################

config.section_('Site')###add a new section of type "Site"
###Site: Grid site parameters are defined in this section, including the stage out information 
###(e.g. stage out destination site, white/black lists, etc.).
config.Site.storageSite       = 'T2_BR_SPRACE'
