# users/utils.py
from suitcases.models import Category, Suitcase

def calculate_jaccard_similarity(user_categories:list[Category], suitcase_categories:list[Category]):
    """
    Calculate Jaccard similarity between user selected categories and suitcase categories. (0-1 float)
    """
    if len(user_categories) == 0:
        return 0.0
    if len(suitcase_categories) == 0:
        return 0.0

    same = 0
    for suit_cat in suitcase_categories:
        for user_cat in user_categories:
            if user_cat == suit_cat:
                same += 1
    different = len(suitcase_categories) 
    
    return same / different 

def get_recommended_suitcases(user_categories:list[Category], max_recommendations:int=3):
    """ Get the recommended suitcases based on jaccard similarity """
    available_suitcases = Suitcase.objects.filter(rented=False).prefetch_related('categories')
    scored_suitcases = []
    for suitcase in available_suitcases:
        suitcase_categories = suitcase.categories.all()
        score = calculate_jaccard_similarity(user_categories, suitcase_categories) * 100.0

        scored_suitcases.append({'suitcase': suitcase, 'score': score, 'categories': suitcase_categories})
    # Sort by score descending
    scored_suitcases.sort(key=lambda x: x['score'], reverse=True)
    return scored_suitcases[:max_recommendations]
