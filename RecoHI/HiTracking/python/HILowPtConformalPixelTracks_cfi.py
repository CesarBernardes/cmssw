import FWCore.ParameterSet.Config as cms

from RecoTracker.TkHitPairs.hitPairEDProducer_cfi import hitPairEDProducer as _hitPairEDProducer
from RecoPixelVertexing.PixelTriplets.pixelTripletHLTEDProducer_cfi import pixelTripletHLTEDProducer as _pixelTripletHLTEDProducer
from RecoPixelVertexing.PixelLowPtUtilities.ClusterShapeHitFilterESProducer_cfi import *
from RecoPixelVertexing.PixelLowPtUtilities.trackCleaner_cfi import *
from RecoPixelVertexing.PixelTrackFitting.pixelFitterByConformalMappingAndLine_cfi import *
from RecoHI.HiTracking.HIPixelTrackFilter_cff import *
from RecoHI.HiTracking.HITrackingRegionProducer_cfi import *

# Hit ntuplets
hiConformalPixelTracksHitDoublets = _hitPairEDProducer.clone(
    clusterCheck = "",
    seedingLayers = "PixelLayerTriplets",
    trackingRegions = "hiTrackingRegionWithVertex",
    maxElement = 0,
    produceIntermediateHitDoublets = True,
)

hiConformalPixelTracksHitTriplets = _pixelTripletHLTEDProducer.clone(
    doublets = "hiConformalPixelTracksHitDoublets",
    maxElement = 5000000, # increase threshold for triplets in generation step (default: 100000)
    produceSeedingHitSets = True,
)

# Pixel tracks
hiConformalPixelTracks = cms.EDProducer("PixelTrackProducer",
                                        
                                        #passLabel  = cms.string('Pixel triplet low-pt tracks with vertex constraint'),
                                        
                                        # Ordered Hits
                                        SeedingHitSets = cms.InputTag("hiConformalPixelTracksHitTriplets"),
                                        
                                        # Fitter
                                        Fitter = cms.InputTag('pixelFitterByConformalMappingAndLine'),
                                        
                                        # Filter
                                        Filter = cms.InputTag("hiConformalPixelFilter"),
                                        
                                        # Cleaner
                                        Cleaner = cms.string("trackCleaner")
                                        )


###Pixel Tracking -  PhaseI geometry

#Tracking regions - use PV from pp tracking
from RecoTracker.TkTrackingRegions.globalTrackingRegionWithVertices_cfi import globalTrackingRegionWithVertices as _globalTrackingRegionWithVertices
hiConformalPixelTracksPhase1TrackingRegions = _globalTrackingRegionWithVertices.clone(RegionPSet=dict(
    precise = True,
    useMultipleScattering = False,
    useFakeVertices       = False,
    beamSpot = "offlineBeamSpot",
    useFixedError = True,
    nSigmaZ = 3.0,
    sigmaZVertex = 3.0,
    fixedError = 0.2,
    VertexCollection = "offlinePrimaryVertices",
    ptMin = 0.3,
    useFoundVertices = True,
    originRadius = 0.2 
))

# SEEDING LAYERS
# Using 4 layers layerlist
from RecoTracker.IterativeTracking.LowPtQuadStep_cff import lowPtQuadStepSeedLayers
hiConformalPixelTracksPhase1SeedLayers = lowPtQuadStepSeedLayers.clone()
hiConformalPixelTracksPhase1SeedLayers.BPix = cms.PSet( 
        HitProducer = cms.string('siPixelRecHits'),
        TTRHBuilder = cms.string('WithTrackAngle'),
)
hiConformalPixelTracksPhase1SeedLayers.FPix = cms.PSet( 
        HitProducer = cms.string('siPixelRecHits'),
        TTRHBuilder = cms.string('WithTrackAngle'),
)


# Hit ntuplets
from RecoTracker.IterativeTracking.LowPtQuadStep_cff import lowPtQuadStepHitDoublets
hiConformalPixelTracksPhase1HitDoubletsCA = lowPtQuadStepHitDoublets.clone(
        seedingLayers = "hiConformalPixelTracksPhase1SeedLayers",
        trackingRegions = "hiConformalPixelTracksPhase1TrackingRegions"
)


from RecoTracker.IterativeTracking.LowPtQuadStep_cff import lowPtQuadStepHitQuadruplets
hiConformalPixelTracksPhase1HitQuadrupletsCA = lowPtQuadStepHitQuadruplets.clone(
	doublets = "hiConformalPixelTracksPhase1HitDoubletsCA",
        CAPhiCut = cms.double(0.2),
        CAThetaCut = cms.double(0.0012),
        SeedComparitorPSet = cms.PSet( 
           ComponentName = cms.string('none')
        ),
        extraHitRPhitolerance = cms.double(0.032),
        maxChi2 = cms.PSet(
           enabled = cms.bool(True),
           pt1 = cms.double(0.7),
           pt2 = cms.double(2),
           value1 = cms.double(200),
           value2 = cms.double(50)
        )
)

#Filter
hiConformalPixelTracksPhase1Filter = hiConformalPixelFilter.clone(
	VertexCollection = cms.InputTag("offlinePrimaryVertices"),
        chi2 = cms.double(999.9),
        lipMax = cms.double(999.0),
        nSigmaLipMaxTolerance = cms.double(999.9),
        nSigmaTipMaxTolerance = cms.double(999.0),
        ptMax = cms.double(999999),
        ptMin = cms.double(0.30),
        tipMax = cms.double(999.0)
)

#Pixel tracks
hiConformalPixelTracksPhase1 = cms.EDProducer("PixelTrackProducer",
    Cleaner = cms.string('pixelTrackCleanerBySharedHits'),
    Filter = cms.InputTag("hiConformalPixelTracksPhase1Filter"),
    Fitter = cms.InputTag("pixelFitterByConformalMappingAndLine"),
    SeedingHitSets = cms.InputTag("hiConformalPixelTracksPhase1HitQuadrupletsCA"),
)




hiConformalPixelTracksSequence = cms.Sequence(
    hiTrackingRegionWithVertex +
    hiConformalPixelTracksHitDoublets +
    hiConformalPixelTracksHitTriplets +
    pixelFitterByConformalMappingAndLine +
    hiConformalPixelFilter +
    hiConformalPixelTracks
)



hiConformalPixelTracksSequence_Phase1 = cms.Sequence(
    hiConformalPixelTracksPhase1TrackingRegions +
    hiConformalPixelTracksPhase1SeedLayers +
    hiConformalPixelTracksPhase1HitDoubletsCA +
    hiConformalPixelTracksPhase1HitQuadrupletsCA +
    pixelFitterByConformalMappingAndLine +
    hiConformalPixelTracksPhase1Filter +
    hiConformalPixelTracksPhase1 
)


