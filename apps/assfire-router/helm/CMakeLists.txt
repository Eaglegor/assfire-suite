include(helm_utils)
prepare_helm_chart(
        CHART_ROOT assfire-router
        VERSION ${${PROJECT_PREFIX}_APPLICATION_RELEASE_VERSION}
)
