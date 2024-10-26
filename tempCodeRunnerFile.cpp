for (auto x : processList)
    {
        cout << x.first << " : ";
        for (auto y : x.second)
            cout << y << " ";
        cout << "\n";
    }
    for (auto x : adjacencyList)
    {
        cout << x.first << " : ";
        for (auto y : x.second)
            cout << y << " ";
        cout << "\n";
    }
    for (auto x : processList)
        cout << x.first << " " << indegree[x.first] << "\n";
    for (auto x : topoOrder)
        cout << x << " ";