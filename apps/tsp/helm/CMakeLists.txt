set (CHART_FILES
        assfire-tsp/Chart.yaml
        assfire-tsp/values.yaml

        assfire-tsp/templates/deployment-server.yaml
        assfire-tsp/templates/deployment-worker.yaml
        assfire-tsp/templates/ingress.yaml
        assfire-tsp/templates/service.yaml
        )

foreach(file ${CHART_FILES})
    configure_file(${file} ${file} @ONLY)
endforeach()