package main

import (
	"assfire.org/api/v1/tsp"
	"context"
	"github.com/google/uuid"
	log "github.com/sirupsen/logrus"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/status"
)

type TspServer struct {
	tsp.UnimplementedTspServiceServer

	taskStorage              *TaskStorage
	solutionStorage          *SolutionStorage
	taskPublisher            *TaskPublisher
	taskQueue                *TaskQueue
	statusEnforcer           *StatusEnforcer
	statusProvider           *StatusProvider
	interruptSignalPublisher *InterruptSignalPublisher
}

func (server *TspServer) StartTsp(ctx context.Context, request *tsp.StartTspRequest) (*tsp.StartTspResponse, error) {
	taskId := uuid.New().String()
	log.Infof("Starting new task %s", taskId)

	enforceError := server.statusEnforcer.enforceStatus(taskId, tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_PENDING)
	if enforceError != nil {
		log.Errorf("Failed to enforce pending status for task %s: %v", taskId, enforceError)
	}

	publishError := server.taskPublisher.publishTask(taskId, request.GetTask())
	if publishError != nil {
		log.Error(publishError)
		enforceError := server.statusEnforcer.enforceStatus(taskId, tsp.WorkerTspStatusUpdate_WORKER_TSP_STATUS_UPDATE_TYPE_ERROR)
		if enforceError != nil {
			log.Error(enforceError)
		}
		return nil, status.Errorf(status.Code(publishError), "Failed to start task %s", taskId)
	}

	return &tsp.StartTspResponse{
		TaskId: taskId,
	}, nil
}

func (server *TspServer) PauseTsp(ctx context.Context, request *tsp.PauseTspRequest) (*tsp.PauseTspResponse, error) {
	log.Infof("Processing pause request for task %s", request.GetTaskId())

	canBePaused, err := server.taskStorage.canBePaused(request.GetTaskId())
	if !canBePaused {
		if err == TaskNotFound {
			return nil, status.Errorf(codes.NotFound, "Task not found: %s", request.GetTaskId())
		} else if err != nil {
			log.Error(err)
			return nil, status.Errorf(status.Code(err), "Failed to pause task %s", request.GetTaskId())
		} else {
			return nil, status.Errorf(codes.Unknown, "Task %s can not be paused now", request.GetTaskId())
		}
	}

	err = server.interruptSignalPublisher.sendPause(request.GetTaskId())
	if err != nil {
		log.Error(err)
		return nil, status.Errorf(status.Code(err), "Failed to pause task %s", request.GetTaskId())
	}

	return &tsp.PauseTspResponse{}, nil
}

func (server *TspServer) ResumeTsp(ctx context.Context, request *tsp.ResumeTspRequest) (*tsp.ResumeTspResponse, error) {
	log.Infof("Processing resume request for task %s", request.GetTaskId())

	canBeResumed, err := server.taskStorage.canBeResumed(request.GetTaskId())
	if !canBeResumed {
		if err == TaskNotFound {
			return nil, status.Errorf(codes.NotFound, "Task not found: %s", request.GetTaskId())
		} else if err != nil {
			log.Error(err)
			return nil, status.Errorf(status.Code(err), "Failed to resume task %s", request.GetTaskId())
		} else {
			return nil, status.Errorf(codes.Unknown, "Task %s can not be resumed now", request.GetTaskId())
		}
	}

	err = server.taskQueue.sendStartTaskSignal(request.GetTaskId())
	if err != nil {
		log.Error(err)
		return nil, status.Errorf(status.Code(err), "Failed to resume task %s", request.GetTaskId())
	}

	return &tsp.ResumeTspResponse{}, nil
}

func (server *TspServer) StopTsp(ctx context.Context, request *tsp.StopTspRequest) (*tsp.StopTspResponse, error) {
	log.Infof("Processing stop request for task %s", request.GetTaskId())

	canBeStopped, err := server.taskStorage.canBeStopped(request.GetTaskId())
	if !canBeStopped {
		if err == TaskNotFound {
			return nil, status.Errorf(codes.NotFound, "Task not found: %s", request.GetTaskId())
		} else if err != nil {
			log.Error(err)
			return nil, status.Errorf(status.Code(err), "Failed to pause task %s", request.GetTaskId())
		} else {
			return nil, status.Errorf(codes.Unknown, "Task %s can not be paused now", request.GetTaskId())
		}
	}

	err = server.interruptSignalPublisher.sendInterrupt(request.GetTaskId())
	if err != nil {
		log.Error(err)
		return nil, status.Errorf(status.Code(err), "Failed to pause task %s", request.GetTaskId())
	}

	return &tsp.StopTspResponse{}, nil
}

func (server *TspServer) GetLatestSolution(ctx context.Context, request *tsp.GetLatestSolutionRequest) (*tsp.GetLatestSolutionResponse, error) {
	log.Infof("Processing latest solution request for task %s", request.GetTaskId())

	solution, err := server.solutionStorage.loadSolution(request.GetTaskId())
	if err == SolutionNotFound {
		return nil, status.Errorf(codes.NotFound, "Solution not found or not yet ready for task %s", request.GetTaskId())
	} else if err != nil {
		return nil, status.Errorf(status.Code(err), "Failed to load solution for task %s", request.GetTaskId())
	}

	return &tsp.GetLatestSolutionResponse{
		Solution: solution,
	}, nil
}

func (server *TspServer) SubscribeForStatusUpdates(request *tsp.SubscribeForStatusUpdatesRequest, observer tsp.TspService_SubscribeForStatusUpdatesServer) error {
	subId := uuid.New().String()
	log.Infof("Processing request to subscribe for status updates, task selector: %v, subscription id: %s", request.GetTaskSelector(), subId)

	ch, err := server.statusProvider.subscribeForStatusUpdates(observer.Context(), request.GetTaskSelector())
	if err != nil {
		log.Error(err)

		return status.Errorf(status.Code(err), "Failed to subscribe for status updates, task selector %v", request.GetTaskSelector())
	}

	for update := range ch {
		log.Debugf("Got an update for task %s: %v", update.GetTaskId(), update.GetType())
		publishError := observer.Send(&tsp.SubscribeForStatusUpdatesResponse{
			StatusUpdate: update,
		})
		if publishError != nil {
			log.Errorf("Failed to publish status update for subscription %s: %v", subId, publishError)
		}
	}

	log.Infof("Subscription %s disposed", subId)
	return nil
}
