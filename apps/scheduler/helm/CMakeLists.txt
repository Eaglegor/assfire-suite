set (CHART_FILES
        assfire-scheduler/Chart.yaml
        assfire-scheduler/values.yaml

        assfire-scheduler/templates/deployment.yaml
        assfire-scheduler/templates/ingress.yaml
        assfire-scheduler/templates/service.yaml
        )

foreach(file ${CHART_FILES})
    configure_file(${file} ${file} @ONLY)
endforeach()